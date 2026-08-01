# Documenting MordoEngine code

Contents:

1. The two comment kinds
2. Documenting a class
3. Documenting a constructor — the `@param` rules
4. Documenting a function
5. Documenting members and structs
6. What never gets documented
7. Reasoning comments in `.cpp` bodies
8. Full worked example
9. Review checklist

**The rules below apply to every header in the engine, whatever subsystem it
belongs to.** The code in the examples is there to show the shape of a good
comment; it is not a claim about what the API currently looks like, and a new
class is documented by the same rules as an old one.

---

## 1. The two comment kinds

**`///` doc comments** go on public API in headers. Visual Studio parses them
into Quick Info and Parameter Help, so a caller hovering a constructor sees
what each argument means without opening the header. This is the entire point:
documentation that arrives at the call site.

**`//` reasoning comments** go everywhere else. They are prose for whoever
reads the file. They never appear in a tooltip and are not meant to.

Do not use `///` inside function bodies. Do not use `//` where a `///` would
have shown up in a tooltip.

Tag style: `@param`, `@return`, `@throws`, `@tparam`, `@note`. Not `\param`.
Never write `@brief` — the first sentence is the brief.

---

## 2. Documenting a class

Every public class and struct gets a brief. One or two sentences that say what
the thing **is** and what its single responsibility is. Not how it works.

If there is a design constraint a user of the class must know — it is
non-copyable, it throws, it must outlive something, its constructor does all
the work — add it as a second paragraph after a blank `///` line.

```cpp
/// One model resident on the GPU: its buffers, one drawable piece per
/// submesh, and the rig every copy of it shares.
///
/// Everything happens in the constructor, so a Model that exists is a Model
/// you can draw. Movable but not copyable, since it owns GL objects.
class Model
{
```

Good briefs answer "what is this?" in the reader's language:

- `/// Anything in the world that has a place in it and may do something with time: the player, an animal, a chest, a lamp.`
- `/// What an actor is allowed to reach for while it updates, handed to it instead of letting it know about World.`
- `/// A heightmap and the operations that read and edit it. Knows nothing about how it gets drawn.`

Bad briefs restate the name and are worse than nothing:

- `/// The Model class.`
- `/// Manages models.`
- `/// This class handles the water system.`

Many existing class comments in the engine are already written this way — they
were just using `//`. Converting one to `///` while working nearby is a
mechanical, high-value change: the prose is right, it simply was not reaching
the tooltip.

---

## 3. Documenting a constructor — the `@param` rules

**Every public constructor that takes parameters gets `@param` for each
parameter whose meaning is not fully determined by its type and name.** This
is the one place to be thorough, because it is the moment the caller most
needs help and the moment they are least likely to open the header.

A `@param` line must add at least one of:

- **Meaning** — what the value represents, not its type
- **Units or space** — degrees or radians, world space or model space, pixels
  or normalized
- **Valid range** and what happens outside it
- **Lifetime** — must outlive the object, is copied, is borrowed
- **Default behaviour** when a default argument exists

If a parameter needs none of these, omit its `@param`. Nine documented
parameters and one obvious one is fine; ten lines of noise is not.

```cpp
	/// Loads the model file and uploads every submesh to the GPU.
	/// @param path     Model file to load, relative to the working directory.
	///                 glTF Separate (.gltf + .bin + textures), never .glb:
	///                 embedded textures cannot be read.
	/// @param textures Library the submesh textures are loaded into and
	///                 borrowed from. Must outlive the Model.
	/// @throws std::runtime_error if the file cannot be read.
	Model(const std::string& path, ResourceLibrary<Texture>& textures);
```

Align the descriptions one space past the longest parameter name. Wrap
continuation lines to the same column.

Defaults are documented as behaviour, not repeated as a value:

```cpp
	/// @param windowWidth  Viewport width in pixels; drives the projection
	///                     aspect ratio. Update it with SetWindowSize on
	///                     resize or the image stretches.
	Camera(glm::vec3 position = glm::vec3(0.0f),
		unsigned int windowWidth = 800,
		unsigned int windowHeight = 600);
```

Never do this:

```cpp
	/// @param path The path.                    // says nothing
	/// @param textures A ResourceLibrary<Texture>&.  // repeats the type
	/// @param loop A bool.                      // repeats the type
```

---

## 4. Documenting a function

Document a public function when **any** of these holds:

- A parameter needs meaning, units, space, range or lifetime explained
- The return value is not obvious from the name — including what a failure or
  empty result looks like
- It throws
- It has a precondition, or must be called in a particular order relative to
  something else
- It is expensive, or cheap in a way that is surprising

Otherwise leave it bare. `void Update(float deltaTime);` on a class whose brief
already says it updates once per frame needs nothing.

```cpp
	/// Plays a clip by name, restarting it from the beginning.
	/// @param name Clip name as it appears in the source file.
	/// @param loop When false the clip holds on its last frame instead of
	///             restarting.
	/// @return false if the file has no clip by that name, leaving the
	///         current clip playing.
	bool PlayAnimation(const std::string& name, bool loop = true);
```

```cpp
	/// Bilinearly samples the heightmap between the four surrounding
	/// vertices, for anything that moves continuously over the terrain
	/// rather than snapping to grid points.
	/// @param x World-space X, not a grid index. Clamped to the terrain.
	/// @param z World-space Z, same.
	float GetHeightInterpolated(float x, float z) const;
```

Complex or important functions — the ones the user actually wants explained —
get more: a short paragraph on the approach, and a note on why the obvious
alternative was not used.

```cpp
	/// Resolves the node globals in a single forward pass and produces one
	/// final matrix per bone.
	///
	/// The loader flattens the hierarchy depth first, so a parent always
	/// precedes its children and one linear sweep is enough; no recursion and
	/// no second pass. A file with no clips falls back to the rest pose
	/// rather than collapsing to the origin.
	/// @param deltaTime Seconds since the last frame. Advances the clock; a
	///                  clip that is not looping stops at its last frame.
	void Update(float deltaTime);
```

That last one is the model to copy for any non-trivial algorithm: what it
does, the property that makes the cheap approach correct, and what happens in
the degenerate case.

---

## 5. Documenting members and structs

Private and protected members use `//`, not `///` — nobody hovers them from
outside. Comment a member only when its state carries meaning:

```cpp
	// Null for a model whose file carried no animation.
	std::unique_ptr<Animator> m_Animator;

	// Held by pointer because the renderer owns GL names and is neither
	// copyable nor movable. That indirection is what keeps this type movable.
	std::unique_ptr<ModelRenderer> m_Renderer;
```

Public plain-data structs get a `///` brief on the struct and a trailing `//`
on each field only when the field is ambiguous:

```cpp
/// What an actor is allowed to reach for while it updates. Built fresh every
/// frame, so it holds references and has no default constructor on purpose.
struct ActorContext
{
	const TerrainSystem& terrain;   // ground height under an actor
	const Camera&        camera;    // where the player is looking
	ActorSystem&         actors;    // spawning, queries, damage
};
```

Sentinels are always documented:

```cpp
	// Node whose live transform applies.
	// -1 = the geometry is already in model space.
	int nodeIndex = -1;
```

---

## 6. What never gets documented

- Trivial accessors: `GetPosition`, `SetName`, `IsAnimated`
- `= default` and `= delete` members, **unless** the reason to delete is
  non-obvious — then document the reason, not the fact
- Destructors that only release what the class obviously owns
- Loop counters, temporaries, anything local and short-lived
- Anything whose comment would be the name with spaces in it

Classes that are mostly small typed accessors are correctly left undocumented
in bulk. A one-line setter whose parameter has an obvious meaning does not
improve with `/// @param value The value.` Leave it, and spend the effort on
the two functions in the file that actually carry a decision.

---

## 7. Reasoning comments in `.cpp` bodies

Inside a function, comment the decision, never the step.

```cpp
	if (!subMesh.diffuseTexturePath.empty())
	{
		// Submeshes commonly share a texture, and ResourceLibrary::Load throws
		// on a duplicate id, so reuse whatever is already there. The library is
		// node based, so the reference stays valid as more models are loaded.
		const std::string& id = subMesh.diffuseTexturePath;
		piece.texture = textures.Exists(id)
			? &textures.Get(id)
			: &textures.Load(id, subMesh.diffuseTexturePath);
	}
```

That comment earns its place: nothing in the code says `Load` throws on
duplicates, or that the reference stays valid.

Compare with a comment that must be deleted:

```cpp
	// Loop over the pieces and render each one
	for (const Piece& piece : m_Pieces)
```

**A constant whose value came from somewhere says where.** A number measured
from an asset, copied from a reference, or tuned by eye is unverifiable
without its origin, and the next reader cannot tell a meaningful value from an
arbitrary one:

```cpp
	// Where the lamp head sits inside the model, taken from the
	// LanternPole_Lantern node in the glTF. Model space, so the actor's scale
	// and yaw carry it.
	const glm::vec3 LANTERN_LIGHT_OFFSET{ -9.582f, 18.009f, 0.0f };
```

---

## 8. Full worked example

Before — works, tells the reader nothing at the call site:

```cpp
class WaterTile
{
public:
	WaterTile(float x, float z, float h, float s);
	void SetTiling(float t);
	float GetHeight() const;
private:
	float m_X, m_Z, m_Height, m_Size, m_Tiling;
};
```

After:

```cpp
/// One square patch of water: where it sits, how big it is, and how often the
/// normal and dudv maps repeat across it. Holds no GL state; PlaneRenderer
/// draws it.
class WaterTile
{
public:
	/// @param centerX     World-space X of the tile centre.
	/// @param centerZ     World-space Z of the tile centre.
	/// @param height      World-space Y of the surface. WaterSystem compares
	///                    the camera against this to decide whether the
	///                    underwater post-process runs.
	/// @param sizeInWorld Edge length in world units, not vertices.
	WaterTile(float centerX, float centerZ, float height, float sizeInWorld);

	/// @param tilingFactor How many times the normal and dudv maps repeat
	///                     across one edge. Higher means smaller ripples.
	void SetTiling(float tilingFactor);

	float GetHeight() const;

private:
	float m_CenterX;
	float m_CenterZ;
	float m_Height;
	float m_SizeInWorld;
	float m_TilingFactor;
};
```

What changed, and what to reproduce on any class: names spelled out, one brief
saying what the thing is, `@param` only where it adds meaning or units, and one
accessor left bare because the brief already covers it.

---

## 9. Review checklist

Reading a diff, in order:

1. Any abbreviation in a new name? Spell it.
2. Any new public class or struct without a `///` brief? Add one.
3. Any new public constructor with parameters and no `@param`? Add them.
4. Any `@param` that only repeats the type or the name? Delete it.
5. Any comment that restates the line below it? Delete it.
6. Any new pointer member without an ownership comment? Add it.
7. Any hard-coded value whose origin is not stated? Say where it came from.
8. Any non-obvious decision with no comment? That is the one comment worth
   writing in the whole diff.
9. Any `///` inside a function body, or `//` where a tooltip was wanted?
   Swap them.
