# Model animation

How model animation works in the engine: which kinds it supports, why the loader
makes the decisions it makes, and what traps sit between Assimp and the GPU.

This document explains *how it works* and links to the code; it does not duplicate
it.

> For the general side — exporting from Blender, textures, placing a model in the
> scene — see **[Loading Blender models](loading-blender-models.md)**.

## Contents

1. [What it covers and what it does not](#1-what-it-covers-and-what-it-does-not)
2. [The `PreTransformVertices` conflict](#2-the-pretransformvertices-conflict)
3. [The data: nodes, bones and channels](#3-the-data-nodes-bones-and-channels)
4. [The `Animator`](#4-the-animator)
5. [Skinning on the GPU](#5-skinning-on-the-gpu)
6. [Four concrete traps](#6-four-concrete-traps)
7. [Exporting animation from Blender](#7-exporting-animation-from-blender)
8. [Common animation problems](#8-common-animation-problems)
9. [Verification](#9-verification)

---

## 1. What it covers and what it does not

A file can carry three different things under the name "animation", and they are
not solved the same way:

| Kind | Status | How it is solved |
|---|---|---|
| **Skeleton** (skinning: joints and per-vertex weights) | ✅ | The `Animator` produces one matrix per bone; the vertex shader blends up to four per vertex. |
| **Rigid node** (a whole piece that moves, with no skeleton) | ✅ | The submesh keeps its `nodeIndex` and `Model::Render` multiplies its transform by that node's live global. |
| **Morph targets** (Blender shape keys) | ❌ | **Not implemented.** It needs *per-vertex positions* blended, not node transforms, so it is a different path: one VBO per target, or a buffer of deltas. Assimp exposes them in `aiMesh::mAnimMeshes` if it is ever wanted. |

The first two share all the machinery, because both reduce to node transforms. The
third does not, and that is why it is absent: it was not just another case of the
same system.

**There is no blending between clips.** Switching animation is a hard cut. Adding
it would mean keeping two times and two poses and interpolating between the two
lists of bone matrices, which is a change contained entirely inside the `Animator`.

---

## 2. The `PreTransformVertices` conflict

This is the part where it is easy to paint yourself into a corner, because **the
flag that makes the static case most convenient is exactly the one that makes the
animated case impossible**.

`aiProcess_PreTransformVertices` bakes the node hierarchy into the vertices. For a
static prop that is perfect: it saves walking the tree and leaves you with a single
draw call. For animation it is fatal, because it **destroys the hierarchy the
animation drives**. The same goes for `aiProcess_OptimizeMeshes`, which merges
meshes from different nodes and breaks the mesh-to-node association.

The way out, in [`ModelLoader.cpp`](../MordoEngine/src/Core/Model/ModelLoader.cpp),
is to decide per file:

1. Read with `ReadFile(path, 0)`, no post-processing, purely to check whether
   `scene->mNumAnimations > 0`.
2. Choose the flags from that.
3. Apply them with `ApplyPostProcessing` **on the same `Importer`**, so the file is
   not parsed twice.

| Case | Its own flags | Why |
|---|---|---|
| Static | `PreTransformVertices` + `OptimizeMeshes` | Nothing will ever move: baking the hierarchy and merging submeshes that share a material is free |
| Animated | `LimitBoneWeights` | Trims each vertex to the four influences the layout can carry, instead of letting the extras be dropped silently |

A consequence of the above: an animated model produces **more submeshes** than the
same model would as a static one, because they are not merged. That is more draw
calls, and it is the right price to pay.

For the static case the loader bakes the node transforms into the vertices itself.
That way the draw path is identical — one call, no hierarchy lookups — and all the
complexity stays in loading.

---

## 3. The data: nodes, bones and channels

In [`ModelData.h`](../MordoEngine/src/Core/Model/ModelData.h):

- **`NodeData`** — the hierarchy, flattened into a vector: name, parent index and
  local transform.
- **`BoneData`** — one joint of the skin: which node drives it, and its inverse
  bind matrix, the one taking a vertex from model space into that bone's local
  space.
- **`NodeChannel`** — the keyframes animating one node, in three independent lists
  (position, rotation, scale). Any of them may be empty.
- **`AnimationData`** — a clip: name, duration and its channels. Times are in
  *ticks*, the unit the file was authored in; divide by `ticksPerSecond` for
  seconds.

And in [`ModelVertex.h`](../MordoEngine/src/Core/Model/ModelVertex.h), every vertex
gains four bone indices and four weights. Unused slots hold bone `-1` and weight
`0`, so **a vertex with no skinning needs no special case in the shader**.

### The flattening trick

The loader flattens the node tree **depth first**, which guarantees a parent always
lands at a lower index than its children. That turns computing the global
transforms into a single forward pass over a vector, with no recursion and nothing
sorted at runtime:

> `global[i] = parent < 0 ? local : global[parent] * local`

It is the reason the flattening is depth first and not breadth first, and the
reason the order of that vector cannot be changed without breaking the `Animator`.

---

## 4. The `Animator`

[`Animator`](../MordoEngine/src/Core/Model/Animation/Animator.h) plays one clip and
produces the matrices the shader needs. Each `Model` has its own, or none if the
file carried no clips.

What it does on every `Update`:

1. Advances the time in ticks, wrapping it with `fmod` if the clip loops. If it
   does not, it stops on the last frame and holds the pose.
2. For each node, samples its channel — linear interpolation on position and scale,
   slerp on rotation — and composes the local matrix.
3. Accumulates the globals in the forward pass described above.
4. Multiplies each bone's global by its inverse bind to get the final matrix:
   `bone[b] = global[bone's node] * inverseBind[b]`.

Three design decisions:

- **The rest pose is decomposed once at construction.** If a clip animates only a
  node's rotation, the position and scale have to come from somewhere; they come
  from the values the file carries in that node's local transform.
- **Channels are indexed by node when `Play` is called.** That makes sampling a
  vector lookup instead of a walk through every channel, per node, per frame.
- **A model with bones but no clips still resolves the rest pose**, so it draws in
  its bind pose instead of collapsing.

`Play` takes the clip name and returns `false` if the file does not have it,
leaving whatever was playing alone. The first clip starts on its own, so it is only
needed to pick a different one.

The matrices are computed **once per frame**, in `Update`, not in `Render`. That
matters because the water passes draw the scene three times: with the computation
in `Render` you would pay it three times over.

---

## 5. Skinning on the GPU

In [`model.vs`](../MordoEngine/res/shaders/model.vs), behind a `skinned` uniform so
a static model pays nothing:

- It walks the four slots, skips the ones holding bone `-1` and accumulates
  `finalBones[id] * weight`.
- **It divides the resulting matrix by the sum of the weights.** That normalises the
  blend, so an export whose weights do not add up to 1 comes out the right size
  instead of shrinking. It works because the last row of an affine matrix is
  `(0,0,0,1)`: the weighted sum leaves it at `(0,0,0,total)` and dividing puts it
  back.
- It applies the same matrix to the normal, so the lighting follows the
  deformation.

On the C++ side,
[`ModelRenderer`](../MordoEngine/src/Renderer/Model/ModelRenderer.cpp) declares the
two new attributes, and
[`Shader::SetMat4Array`](../MordoEngine/src/Core/Shader/Shader.h) uploads the whole
skeleton at once: the elements of a uniform array are contiguous, so the location
of element 0 plus a count is enough. Without it there would be as many
`glGetUniformLocation` calls as bones, every frame and per model.

---

## 6. Four concrete traps

1. **Assimp is row major, glm is column major.** Converting an `aiMatrix4x4` is a
   **transpose**, not a `memcpy`. Get it wrong and the skeleton explodes into
   spikes running off to infinity; it is the classic symptom and it looks like
   nothing else.
2. **Bone indices need `glVertexAttribIPointer`.** Through the ordinary
   `glVertexAttribPointer` they arrive converted to float and the shader indexes
   the wrong bones.
3. **`MAX_BONES` lives in two places** and they have to agree:
   [`ModelVertex.h`](../MordoEngine/src/Core/Model/ModelVertex.h) and
   [`model.vs`](../MordoEngine/res/shaders/model.vs). Each is commented as
   referring to the other. A hundred matrices is 1600 uniform components, and the
   minimum GL 3.3 **guarantees** is 1024: it works on any desktop GPU, but on very
   tight hardware it would have to come down.
4. **A bone whose node is missing** would render as identity and deform the mesh
   **without raising any error**. It is worth checking on import that this counter
   comes out at zero.

---

## 7. Exporting animation from Blender

On top of the
[general checklist](loading-blender-models.md#checklist-before-exporting), with one
important difference and three additions:

- **Do not apply *All Transforms* to the armature.** In the static case baking the
  transforms is what you want; here they are exactly what the animation needs. It
  is the one row of the general checklist that inverts.
- **Data → Armature:** *Deformation Bones only* if you want fewer bones.
- **Animation:** tick `Animations` and, if you have several actions, *Group by NLA
  Track* so they come out as clips with their own names.
- **At most four influences per vertex.** In Blender, *Weight Paint* → *Limit
  Total* with a limit of 4 leaves the mesh matching what the layout can carry, and
  spares the importer from having to discard weights.

The name you pass to `Model::PlayAnimation` is the action or clip name. If you get
it wrong, the engine lists the ones the file does have on the console.

---

## 8. Common animation problems

| Symptom | Likely cause |
|---|---|
| The skeleton explodes into spikes running to infinity | The `aiMatrix4x4` → `glm::mat4` conversion does not transpose. |
| The animated model shows up in its rest pose, motionless | The file carried no clips, or nobody calls `ModelSystem::Update`. |
| The animated model collapses into a point | The weights never arrived: check whether `boneIds` is all `-1`, or whether `MAX_BONES` was too low. |
| The animation runs at double or half speed | The file's `ticksPerSecond` is 0 and the default of 25 is being used. |
| Parts of the model lag behind when animating | A rigid mesh in an animated file with no `nodeIndex`, or `PreTransformVertices` was applied to it. |
| Only a patch deforms, near a joint | Weights were discarded for exceeding four influences. Limit them in Blender. |
| The clip name does not work | `PlayAnimation` returned `false`. Check the list the engine prints on load. |

The general import symptoms are in
[Loading models → Common problems](loading-blender-models.md#7-common-problems).

---

## 9. Verification

`ModelLoader` does not depend on OpenGL, so the data side can be checked with no
window and no context (see
[Loading models → Quick verification](loading-blender-models.md#8-quick-verification)
for the compile line). What to look at in the `ModelData`:

| Value | What to expect |
|---|---|
| Node, bone and clip counts | Not zero, in a file that does carry animation |
| The sum of each skinned vertex's weights | **Exactly 1**. Anything else means influences were lost |
| Vertices with any influence | All of them, in a skinned mesh |
| Bones with `nodeIndex < 0` | **Zero**. Any other value is trap 4 |
| Each clip's duration in seconds | Should match what it lasts in Blender |

And at runtime, that the animation **advances**: that the `Animator`'s time grows
and wraps exactly at the clip's duration. Two screenshots a fraction of a second
apart have to show different poses — that it compiles and that something shows up
does not prove it is animating.
