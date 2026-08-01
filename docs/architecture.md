# MordoEngine architecture conventions

Read this before adding a subsystem, a renderer, an actor, or anything that
touches OpenGL. These are the patterns the engine converged on; new code that
ignores them creates the coupling the last several refactors removed.

**These rules are about shape, not about which subsystems exist.** Types named
below are the instances that happen to exist today — they illustrate a pattern
that the next subsystem is expected to follow too. Nothing here needs editing
when something new is added; if a new subsystem seems to need an exception,
that is the thing to question, not this file.

---

## The three-way split

Every subsystem that puts geometry on screen is split into three pieces that
never leak into each other:

| Piece | Owns | Knows about GL |
|:--|:--|:--|
| `XLoader` / `XData` | plain CPU data | **no** |
| `XRenderer` | GPU buffers and GL names | yes |
| `XSystem` | the collection, the per-pass uniforms | no |

The loader produces data. The renderer uploads it and issues draws. The system
owns the objects, drives their `Update`, and sets the uniforms that are the
same for the whole pass.

Why it is worth the extra type: the loader is testable without a GL context,
the renderer is the only thing that has to be non-copyable, and swapping the
backend touches one layer.

Not every subsystem needs all three. One that owns no CPU-side data of its own
skips the loader; one that draws nothing skips the renderer. What is not
optional is the direction of the dependencies: data never reaches for the
renderer, and the renderer never reaches for the system.

Placement: `src/<Subsystem>/` for the data and the system, with the renderer
under `src/Renderer/<Subsystem>/`. Systems live in a `System/` subfolder when
the subsystem has more than one file.

---

## Uniform scope

Split shader uniforms by how often they change:

- **Per pass**, set once by the system: projection, view, clip plane, light
  uniforms, shared samplers.
- **Per object**, set by the object itself: model matrix, per-instance flags,
  bone matrices.

A system sets the per-pass uniforms once and then loops; each object sets only
what is its own. Do not let an object set a per-pass uniform — it will be set N
times per frame for no reason, and the next reader will not know which one
wins.

---

## Contexts, not World

Nothing downstream of `World` knows what `World` is. It receives a context
struct instead. The ones that exist today:

- `EngineContext` — owns the resource libraries and the world; lives for the
  program.
- `RenderContext` — what a renderer needs to draw this pass.
- `ActorContext` — what an actor may reach for while it updates.

Per-frame contexts are **built fresh in `Update` / `Render`**, hold references
rather than pointers or copies, and deliberately have no default constructor.
That is what makes it impossible to hold one past the frame it belongs to.

**Giving something a new dependency means adding a reference to the context it
already receives** — never passing `World` down, never a global, and never a
back-pointer to the owner. When a context grows a field, every consumer keeps
compiling and the new dependency is visible in one place.

---

## A render path runs more than once per frame

`World::RenderOpaque` is invoked once per pass that needs the opaque scene.
Today that is three times — the water reflection, the water refraction and the
screen — but the number is a property of which passes currently exist, and
shadow maps or a second water body would change it. Write as if the count were
unknown.

Therefore:

- No animation advance, no gameplay, no state mutation, no allocation, no
  spawning, no input reading inside a render path.
- All of that goes in `Update`, which runs once.
- The actor layer exists partly to enforce this: an actor never draws itself.
  The actor system runs gameplay; the rendering systems draw, unaware of
  actors.

If a change needs per-frame work that is currently in a render path, move the
work to `Update` and cache the result — do not add a "first pass only" flag.

Debug and overlay drawing is the one thing that belongs *outside*
`RenderOpaque` rather than inside it: draw it once, after the scene is
resolved, so it is not multiplied by the passes and does not appear in a
reflection.

---

## OpenGL containment

Raw `gl*` calls are allowed only in:

- `src/Renderer/**`
- `src/API/OpenGL/**`

Everything else — `World`, systems, actors, scenes — calls renderers. A new
subsystem that needs to touch GL gets a renderer under `src/Renderer/`; that is
what the layer is for.

Where a GL object also carries setup state, the setup belongs with it rather
than with the caller: binding a framebuffer also sets its viewport and clears
it, so no caller ever sets a viewport itself.

---

## Ownership patterns

**GL-owning types delete copy and move.** A type that holds a GL name — a
shader program, a VAO, a framebuffer — must not be copied, because the copy
would double-free it.

**To store a GL-owning type in a container, put it behind `unique_ptr`.** The
owning type stays movable and can live in a `std::vector`. Comment this
whenever it appears — it looks like a stray indirection and it is not.

**Any container whose elements are pointed at from outside holds them by
`unique_ptr`.** A `std::vector<T>` reallocates, and every reference handed out
before that becomes dangling. If callers keep a pointer or a reference to an
element, the element must not move: store `std::unique_ptr<T>`, or use a
node-based container.

**Resources are borrowed, never owned twice.** Shared assets live in a
`ResourceLibrary`; everything else holds a non-owning pointer or a `const`
reference to them. `ResourceLibrary::Load` throws on a duplicate id, so check
`Exists` first when several callers may want the same resource.

**A raw pointer member means optional and non-owning.** Anything else is a
`unique_ptr` (owned) or a reference (required, non-owning). Document what
`nullptr` means every time.

**Split shared asset from per-instance state.** When several objects use the
same loaded thing, what is identical across them — buffers, textures, the data
read from the file — belongs to one shared object, and what differs — where it
is, what it is currently doing — belongs to one small per-instance object that
borrows it. Applied to models this is the `Model` / `Body` split; the same
reasoning applies to anything else that gets instanced.

---

## Namespaces

The engine has no named namespaces. The last one was removed on purpose: it
wrapped a single unambiguous class hierarchy and bought nothing.

Instead:

- Nest a helper type inside the class that owns it.
- Put a shared constant on the class as `static constexpr`.
- Use an **anonymous namespace** in a `.cpp` for file-local constants and
  helper functions. That is the right home for scene-specific paths, tuning
  values and one-off maths.

Never `using namespace` in a header. Never in a `.cpp` either.

---

## The Visual Studio project does not glob

`MordoEngine.vcxproj` lists every source file explicitly. A file that exists on
disk and is not listed is simply not compiled, so the failure arrives as an
unresolved external at link time, pointing at the caller rather than at the
missing file.

Every new file needs **two** entries. First the project itself, which is what
decides whether the file is compiled at all:

```xml
<!-- MordoEngine.vcxproj -->
<ItemGroup>
  <ClInclude Include="src\Subsystem\Name.h" />
</ItemGroup>
<ItemGroup>
  <ClCompile Include="src\Subsystem\Name.cpp" />
</ItemGroup>
```

Then the filters file, which only decides where the file appears in Solution
Explorer. This project uses the three default Visual Studio filters and nothing
else — the IDE tree is grouped by kind, not mirrored from `src/`:

```xml
<!-- MordoEngine.vcxproj.filters -->
<ClInclude Include="src\Subsystem\Name.h">
  <Filter>Archivos de encabezado</Filter>
</ClInclude>
<ClCompile Include="src\Subsystem\Name.cpp">
  <Filter>Archivos de origen</Filter>
</ClCompile>
```

Headers go under `Archivos de encabezado`, sources under `Archivos de origen`;
`Archivos de recursos` is the third and is not used by engine code. Do not
invent a per-subsystem filter for one new file — every existing entry uses one
of these three, and a lone custom filter just makes the tree inconsistent.

Omitting the filters entry still builds, so the mistake is invisible until
someone opens the IDE and finds the file loose at the root. Add both.

Header-only additions still need their `ClInclude`, or the header never shows
up in the IDE at all.

---

## Adding a new subsystem — checklist

1. Does it load or hold data? Write `XLoader` producing `XData`, with zero GL
   calls, so it can be exercised without a window.
2. Does it draw? Write `XRenderer` under `src/Renderer/X/`, delete copy and
   move, own the VAO/VBO/EBO.
3. Does it own a collection or per-pass state? Write `XSystem`.
4. Does it need something it cannot reach? Add a reference to the relevant
   context struct; do not pass `World` and do not add a back-pointer.
5. Does it need a shader or a shared asset? Register it in `ResourceLoader`.
   Assets loaded per object stay out of the startup load and load on demand,
   keyed by path.
6. Does it run every frame? Decide once whether the work belongs in `Update` or
   a render path, remembering a render path runs once per pass.
7. Is anything pointed at from outside its container? Store it by `unique_ptr`.
8. **Register every new file** in `MordoEngine.vcxproj` and
   `MordoEngine.vcxproj.filters`.
9. Does it need a project-level change (a vendored library, a define, a DLL to
   copy)? Wire `Debug|x64` and `Release|x64` both, and add the post-build copy
   step next to the existing ones.
