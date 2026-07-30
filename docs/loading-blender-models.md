# Loading Blender models in MordoEngine

How to get an object modelled in Blender into the scene, using Assimp
(`vendor/assimp`, v6.0.5) and respecting the same split the terrain already uses:
**data and technique with no GL** → **renderer with GL** → **system that drives them**.

This document explains *how it works* and links to the code; it does not duplicate
it. If you need the exact signature of something, the link takes you to the file.

> Animation (skeletons, clips, skinning) has its own document:
> **[Model animation](model-animation.md)**.

## Contents

1. [Overview](#1-overview)
2. [Exporting from Blender](#2-exporting-from-blender)
3. [The two conventions that bite](#3-the-two-conventions-that-bite)
4. [What each piece does](#4-what-each-piece-does)
5. [Transforms](#5-transforms)
6. [Wiring it into World](#6-wiring-it-into-world)
7. [Common problems](#7-common-problems)
8. [Quick verification](#8-quick-verification)
9. [Appendix: `.glb` with embedded textures](#9-appendix-glb-with-embedded-textures)

---

## 1. Overview

```
Blender  →  res/models/<name>/<name>.gltf
              ↓  ModelLoader        (Assimp → CPU, not a single GL call)
            ModelData
              ↓  ModelRenderer      (VAO/VBO/EBO, emits MeshDrawCall)
            Model                   (mesh + materials + transform)
              ↓
            ModelSystem             (the collection + the per-pass uniforms)
              ↓
            World::RenderOpaque
```

This is the same split as `Geomipmapping` (technique) / `TerrainMeshRenderer` (GL)
/ `TerrainSystem` (director). The practical consequence is that `ModelLoader` does
not depend on OpenGL, so it can be compiled and exercised on its own, with no
window and no context: that is what the [quick verification](#8-quick-verification)
relies on.

### The files

| File | What it is |
|---|---|
| [`Core/Model/ModelVertex.h`](../MordoEngine/src/Core/Model/ModelVertex.h) | The vertex layout and the two skinning limits |
| [`Core/Model/ModelData.h`](../MordoEngine/src/Core/Model/ModelData.h) | A model as plain CPU data |
| [`Core/Model/ModelLoader.h`](../MordoEngine/src/Core/Model/ModelLoader.h) · [`.cpp`](../MordoEngine/src/Core/Model/ModelLoader.cpp) | File → `ModelData`, through Assimp |
| [`Renderer/Model/ModelRenderer.h`](../MordoEngine/src/Renderer/Model/ModelRenderer.h) · [`.cpp`](../MordoEngine/src/Renderer/Model/ModelRenderer.cpp) | The GPU buffers and the draw calls |
| [`Core/Model/Model.h`](../MordoEngine/src/Core/Model/Model.h) · [`.cpp`](../MordoEngine/src/Core/Model/Model.cpp) | A model already on the GPU, with its place in the world |
| [`Core/Model/System/ModelSystem.h`](../MordoEngine/src/Core/Model/System/ModelSystem.h) · [`.cpp`](../MordoEngine/src/Core/Model/System/ModelSystem.cpp) | The collection of models and the shared uniforms |
| [`Core/Transform/Transform.h`](../MordoEngine/src/Core/Transform/Transform.h) · [`.cpp`](../MordoEngine/src/Core/Transform/Transform.cpp) | Position, rotation and scale |
| [`res/shaders/model.vs`](../MordoEngine/res/shaders/model.vs) · [`model.fs`](../MordoEngine/res/shaders/model.fs) | The model shaders |
| [`Core/Model/Animation/Animator.h`](../MordoEngine/src/Core/Model/Animation/Animator.h) · [`.cpp`](../MordoEngine/src/Core/Model/Animation/Animator.cpp) | See [Model animation](model-animation.md) |

---

## 2. Exporting from Blender

### Recommended format: glTF 2.0 — *Separate* (`.gltf` + `.bin` + textures)

Pick **glTF 2.0** and, inside the dialog, **glTF Separate**, not `.glb`. Two
concrete reasons for this engine:

1. **glTF is Y-up by specification.** Blender's exporter converts Z-up → Y-up
   automatically (the `+Y Up` option, on by default). Nothing has to be rotated in
   code. With FBX or OBJ you have to deal with the axis conversion by hand.
2. **`Separate` leaves the textures as loose files on disk.** The
   [`Texture`](../MordoEngine/src/Core/Texture/Texture.cpp) class only knows how to
   build itself from a path, so the embedded textures of a `.glb` **cannot be
   loaded** without modifying it. With `Separate` it works as things stand today.
   (If you ever want `.glb`, see the
   [appendix](#9-appendix-glb-with-embedded-textures).)

### Checklist before exporting

| Step | Why |
|---|---|
| `Ctrl+A` → **All Transforms** | Bakes scale 1 and rotation 0 into the mesh. Otherwise the transform lives on the node and the model shows up moved or scaled. |
| Apply modifiers (or tick *Apply Modifiers* on export) | Assimp reads geometry, not Blender's modifier stack. |
| Check the normals: `Shift+N` (*Recalculate Outside*) | Flipped normals mean black or invisible faces once back-face culling is on. |
| At least **one UV map** | With no UVs every vertex comes out at `texCoord = (0,0)` and the model is painted with a single texel. |
| A **Principled BSDF** material with *Base Color* → *Image Texture* | That is what Assimp exposes as `BASE_COLOR` / `DIFFUSE`. A flat colour with no texture produces none. |
| Scale: 1 Blender unit = 1 engine unit | Check it against `TerrainSystem::GetTerrainWorldScale()`. |

> If the model is going to be animated, the first row does **not** apply to the
> armature. See
> [Model animation → Exporting from Blender](model-animation.md#7-exporting-animation-from-blender).

### Export dialog settings

- **Format:** `glTF Separate (.gltf + .bin + textures)`
- **Include:** `Selected Objects` if you only want one object
- **Transform:** `+Y Up` ✔
- **Data → Mesh:** `Apply Modifiers` ✔, `UVs` ✔, `Normals` ✔
- **Data → Material:** `Export`
- **Compression (Draco):** ❌ **off** — it requires Assimp to have been built with
  Draco support; without it the mesh arrives empty and no clear error is raised.

Save it under `res/models/<whatever>/`, next to `res/maps` and `res/shaders`. The
paths the engine loads are the constants at the top of
[`World.cpp`](../MordoEngine/src/World/World.cpp); today they point at the two
sample models from the Khronos sample assets. Swap that line for your own export.

---

## 3. The two conventions that bite

### 3.1 The UV flip

[`Texture.cpp`](../MordoEngine/src/Core/Texture/Texture.cpp) already calls
`stbi_set_flip_vertically_on_load(true)` when it is constructed. That is why you
must **not** pass `aiProcess_FlipUVs` to Assimp: apply both and they cancel out,
leaving the texture upside down.

The LearnOpenGL tutorials do use it, because there the stb flip is off. This is the
number one bug when bolting Assimp onto an engine whose textures already worked.

### 3.2 `ResourceLibrary::Load` throws if the id already exists

[`ResourceLibrary`](../MordoEngine/src/Core/Resources/ResourceLibrary.h) uses
`try_emplace` and throws `std::runtime_error` if the id was taken. A model with
several meshes usually repeats the same texture, so `Exists()` has to be consulted
before loading or the second submesh kills the program. `Model` already does this.

Because the library is an `unordered_map`, the `Texture*` each model keeps stay
valid even as more textures are loaded later: inserting only invalidates iterators,
not references.

---

## 4. What each piece does

### 4.1 The data

[`ModelVertex.h`](../MordoEngine/src/Core/Model/ModelVertex.h) defines the layout
— position, UV, normal and the four skinning slots — the same way
`TerrainVertex.h` does: the layout belongs to neither the loader nor the renderer,
but to both.

[`ModelData.h`](../MordoEngine/src/Core/Model/ModelData.h) describes a model as
plain CPU data: a list of submeshes, each with its geometry, its diffuse texture
path and two fields that only matter when animating (the node carrying it and
whether it is skinned). The node hierarchy, the bones and the clips live here too.

### 4.2 The loader

[`ModelLoader.cpp`](../MordoEngine/src/Core/Model/ModelLoader.cpp) turns a file
into `ModelData` and contains **not a single OpenGL call**. Beyond reading vertices
and indices, it:

- **Resolves texture paths** against the model's own folder. The `.gltf` stores
  them relative, so the executable's working directory stops mattering.
- **Reads the PBR slot first** (`aiTextureType_BASE_COLOR`) and falls back to the
  legacy diffuse one (`aiTextureType_DIFFUSE`). That covers glTF and older formats
  with the same code.
- **Chooses its post-processing flags per file.** This is the least obvious
  decision in the loader and it is explained in
  [Animation → The `PreTransformVertices` conflict](model-animation.md#2-the-pretransformvertices-conflict).
- **Bakes the node transforms** into the vertices when the model is static, so
  drawing it never has to consult a hierarchy.

It throws `std::runtime_error` if the file cannot be read. A broken file is not
swallowed silently.

### 4.3 The renderer

[`ModelRenderer`](../MordoEngine/src/Renderer/Model/ModelRenderer.h) is modelled on
`TerrainMeshRenderer` and reuses the same
[`MeshDrawCall`](../MordoEngine/src/Renderer/MeshDrawCall.h): **every submesh goes
into a single VBO and EBO**, and each one is a slice drawn with
`glDrawElementsBaseVertex`. One more submesh does not mean one more buffer.

Two things worth knowing before touching it:

- **It owns exactly one VAO/VBO/EBO and `Upload` overwrites them** with
  `glBufferData`. Two models cannot share a renderer: the second would clobber the
  first one's geometry. It is one `ModelRenderer` per `Model`.
- **It is neither copyable nor movable.** It declares its copy constructor
  `delete`, and that also suppresses the move constructor.

### 4.4 `Model`

[`Model`](../MordoEngine/src/Core/Model/Model.h) is a model already resident on the
GPU: its buffers, the draw call and texture of each submesh, its place in the world
and, if the file carried one, the animation currently playing. All the work happens
in the constructor, so **a `Model` that exists is a `Model` you can draw**.

The detail that is not cosmetic: `m_Renderer` is held through a `unique_ptr`
precisely because `ModelRenderer` is not movable. That indirection is the only
thing keeping `Model` movable, and therefore storable in a `std::vector` — without
it, `ModelSystem` would not compile.

Submeshes are kept as a list of `Piece`, with the draw call, the texture, the node
index and the skinning flag together, rather than four parallel vectors that would
have to be kept aligned by hand.

### 4.5 `ModelSystem`

[`ModelSystem`](../MordoEngine/src/Core/Model/System/ModelSystem.h) holds no
geometry: it holds models and sets the uniforms they share. A system with zero
models is a valid state — it is constructed empty and `Render` does nothing — so
`World` needs no pointers and no null checks.

**The way the uniforms are split is the point of this pair of classes.**
`projection`, `view`, `plane` and `texture1` are the same for every model in the
pass, and are set **once** in `ModelSystem::Render`. The only thing that changes
from one model to the next is its `model` matrix, and `Model::Render` sets that. If
they all lived inside the loop you would pay four `glUniform` calls per model for
nothing.

`Add` returns a `Model&` with the same contract as `std::vector`: **the reference
is invalidated by the next call to `Add`**. Use it right away or do not keep it.

### 4.6 The shaders

[`model.vs`](../MordoEngine/res/shaders/model.vs) applies the same clip plane as
the terrain, which is what makes models show up in the water reflection and
refraction passes, and it holds the skinning path.

[`model.fs`](../MordoEngine/res/shaders/model.fs) reuses the structs and functions
of `terrain.fs` as they are, with the same uniform names, so `LightSystem` can feed
it without changing anything.

> If a model carries **non-uniform** scale, `mat3(model)` is not good enough for
> the normals: it would need `transpose(inverse(mat3(model)))` or they come out
> skewed. With `Transform::SetScale(float)` the scale is uniform and the problem
> does not arise.

### 4.7 The `LightSystem` change

`LightSystem::ApplyUniforms` was **private**, and `Render()` only applied it to the
terrain shader. Without touching that, any model draws **completely black**,
because `dirLight` and `pointLights[]` stay at zero.

The change was moving its declaration into the public section of
[`LightSystem.h`](../MordoEngine/src/Lighting/LightSystem.h). The `.cpp` needed
nothing.

---

## 5. Transforms

A `Model` does not hold a loose `glm::mat4`, it holds a
[`Transform`](../MordoEngine/src/Core/Transform/Transform.h). That makes changing
the height or the size of something a one-liner instead of recomposing a matrix:

```cpp
Transform& t = model.GetTransform();
t.SetPosition({ x, terrainHeight, z });
t.SetScale(0.35f);        // uniform, no distortion
t.SetYaw(-120.0f);        // degrees
t.SetHeight(y + 5.0f);    // height only, x and z untouched
```

Three decisions worth knowing about:

- **Rotation is in degrees**, not radians. That is the unit you think in when
  placing something by hand; the conversion happens inside `GetMatrix`.
- **The order is translation → yaw → pitch → roll → scale.** With scale last it
  does not leak into the rotation, which is what produces those odd shears when
  you scale and rotate the other way round.
- **The matrix is cached behind an `m_Dirty` flag.** This is not premature
  optimisation: the water reflection and refraction passes redraw the scene, so
  `GetMatrix()` is called several times per frame per object.

---

## 6. Wiring it into `World`

Loading a model is **optional**: with nothing on disk the engine still starts and
just says so on the console.

What has to change, all in [`World`](../MordoEngine/src/World/World.cpp):

| Where | What |
|---|---|
| `World.h` | A `ModelSystem m_Models` member and a `Shader& m_ModelShader` reference |
| Initialiser list | `m_ModelShader(shaders.Get("model"))`. `m_Models` does **not** go here: it is default constructed empty |
| Constructor body | Check the file exists and call `m_Models.Add(...)` |
| `World::Update` | `m_Models.Update(deltaTime)` |
| `World::RenderOpaque` | `m_Lights.ApplyUniforms(m_ModelShader, ...)` and then `m_Models.Render(...)` |

Four things that matter and do not show up in a diff:

1. **The order of the initialiser list** has to match the declaration order of the
   members, or MSVC warns with C5038.
2. **Models are drawn in `RenderOpaque`, not in `Render`.** `RenderOpaque` is the
   callback the water reflection and refraction passes invoke; put them in `Render`
   and they will not appear in the water.
3. **They go before the sky.** The skybox is drawn last with its depth trick, so
   any opaque geometry has to come first.
4. **The existence check lives in `World`**, because "no model yet" is a normal
   state for the scene. But if the file is there and Assimp cannot read it,
   `LoadModel` throws and the program dies with the message: that one really is an
   error.

Adding more models is calling `Add` again, even with the same path. The `Model`
objects live in a `std::vector`, so they get moved when it reallocates; that works
because the renderer's `unique_ptr` moves with them and nobody frees the GL names
twice.

### Placing the model on the terrain

`World` uses a helper that looks up the terrain height at (x, z) and drops the
model there. Two warnings:

- **If the model's pivot is at its centre** instead of its base, it will sink
  halfway in. Check it with the `min.y` of its bounding box: if it is not ~0, add
  that offset with `SetHeight`.
- **Watch the water level.** On the current terrain the centre — where the camera
  spawns — is at height 0 and the water sits at `0.3 * heightScale`, that is 300,
  so a model placed at the centre ends up at the bottom of the lake. That is why
  the two sample ones are on the eastern shore.

As a scale reference, the point light cubes are drawn at 20 units
(`PointLight.cpp:28`).

### Registering the shader

In [`ResourceLoader.cpp`](../MordoEngine/src/Core/Resources/ResourceLoader.cpp),
next to the others, under the id `"model"`.

Model textures do **not** go into `LoadTextures`: each `Model` loads them on
demand, keyed by the path Assimp returns.

### Adding the files to the `.vcxproj`

The project is MSBuild, not CMake, so new files have to be declared by hand as
`ClCompile` (the `.cpp` files), `ClInclude` (the headers), `None` (shaders and the
`.gltf`/`.bin`) and `Image` (textures). The assets do not affect compilation, but
listing them makes them show up in the Solution Explorer like everything else.

---

## 7. Common problems

| Symptom | Likely cause |
|---|---|
| Texture upside down | You passed `aiProcess_FlipUVs` **and** `Texture` already does `stbi_set_flip_vertically_on_load(true)`. Drop the flag. |
| Model completely black | `LightSystem::ApplyUniforms` was not called on the model shader ([4.7](#47-the-lightsystem-change)). |
| Model white or flat with no texture | The Blender material had no *Image Texture* in *Base Color*, or you exported without materials. |
| `Resource already loaded: ...` | Two submeshes share a texture and `Exists()` was not consulted before `Load()`. |
| Crash on startup with an odd path | The `.gltf` stores relative paths and `FileSystem::getPath` resolves against the working directory. Launch the executable from the project directory. |
| Empty mesh with no error at all | You exported with **Draco** compression. Turn it off. |
| Model huge or microscopic | *All Transforms* was not applied, or the scene's *Unit Scale* is not 1. |
| Faces disappearing depending on the angle | Flipped normals: in Blender, `Shift+N`. |
| Model lying down 90° | Exported with `+Y Up` off, or OBJ/FBX with no axis conversion. |
| Model missing from the water reflection | You drew it in `Render()` instead of `RenderOpaque()`. |
| Normals skewed when scaling | Non-uniform scale with `mat3(model)`. See [4.6](#46-the-shaders). |

The symptoms specific to animation are in
[Animation → Common animation problems](model-animation.md#8-common-animation-problems).

---

## 8. Quick verification

Because `ModelLoader` does not depend on OpenGL, it can be compiled **on its own**
against a throwaway `main` and the import checked with no window, no context and no
shaders. It is the fastest way to tell an import problem apart from a GL one:

```
cl /std:c++20 /EHsc /MD /DASSIMP_DLL /I<vendor/assimp/include> /I<vendor/glm> \
   check.cpp src/Core/Model/ModelLoader.cpp /link /LIBPATH:<vendor/assimp/lib> assimp-vc143-mt.lib
```

What is worth printing out of the resulting `ModelData`:

| Value | What it tells you |
|---|---|
| Submesh, vertex and index counts | That the geometry arrived |
| `diffuseTexturePath` and whether it exists on disk | That path resolution worked |
| The first `texCoord` | If every one reads `(0,0)`, the model had no UVs |
| The bounding box | The real scale, and where the pivot is (`min.y` ~0 means at the base) |

If those numbers are coherent, whatever is left is a GL problem and not an import
one — and that narrows the search a great deal. For the animation data, see
[Animation → Verification](model-animation.md#9-verification).

---

## 9. Appendix: `.glb` with embedded textures

Not supported today. These are the two pieces that would be missing:

1. **A `Texture` constructor that loads from memory.** `stbi_load` reads from a
   file; a buffer already in RAM needs `stbi_load_from_memory`.
2. **Detecting the embedded texture in the loader.** Assimp returns paths starting
   with `*` in that case, and the content comes out of
   `scene->GetEmbeddedTexture(path)`. Careful: in the resulting `aiTexture`, if
   `mHeight == 0` then `mWidth` is a count of *bytes* of a compressed file, not
   pixels.

Until then, `glTF Separate` avoids all of it.
