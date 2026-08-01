# MordoEngine C++ style

House rules for MordoEngine, derived from the engine's own code. When this
guide and a neighbouring file disagree, prefer this guide and note the drift,
unless the file is on the legacy list below.

**Every rule here is written to apply to any subsystem, including ones that do
not exist yet.** Where a real type is named, it is an illustration of the rule,
never the extent of it. A new subsystem is not an exception to anything in this
file; it is the next thing the file describes.

**Before writing anything**, read one or two files next to the one being
touched. Match their shape.

## The one principle

Code says *what*. Comments say *why*. A comment that restates the code is a
defect: delete it or rewrite the code so the comment becomes unnecessary.

Two kinds of comment, never mixed up:

| Kind | Syntax | Where | Purpose |
|:--|:--|:--|:--|
| Doc comment | `///` with `@param` | public API in headers | Shows in Visual Studio Quick Info and Parameter Help, so the caller sees what an argument means before typing it |
| Reasoning comment | `//` | anywhere else | Ownership, invariants, why a design was forced, non-obvious tradeoffs |

Read [documenting-code.md](documenting-code.md) before writing or reviewing any header
documentation. It has the full rules and worked examples.

## Naming

| Element | Convention | Example |
|:--|:--|:--|
| Class, struct, enum | `PascalCase`, no type prefix | `WaterSystem`, `ModelRenderer` |
| File | exactly the type it declares | `WaterSystem.h` / `WaterSystem.cpp` |
| Function, method | `PascalCase` verb phrase | `PlaceOnTerrain`, `MarkVertexAsModified` |
| Private member | `m_PascalCase` | `m_Animator`, `m_PendingDestroy` |
| Protected member | `p_PascalCase` | `p_HeightData`, `p_WorldScale` |
| Public data member of a plain struct | `camelCase`, no prefix | `nodeIndex`, `drawCall`, `terrain` |
| Local variable, parameter | `camelCase` | `deltaTime`, `heightOffset` |
| Constant (`static constexpr`, file-scope `const`) | `SCREAMING_SNAKE_CASE` | `MAX_BONES`, `DEFAULT_REACH` |
| Boolean | reads as a question | `IsAnimated`, `HasModifications`, `p_IsScaled` |
| Accessors | `GetX` / `SetX` | `GetHeightScale`, `SetWorldScale` |

### Names are the primary documentation

**Never abbreviate.** A shortened name saves four keystrokes once and costs a
reader a lookup every time. Spell it:

| Never | Always |
|:--|:--|
| `ctx` | `context` |
| `pos`, `rot`, `scl` | `position`, `rotation`, `scale` |
| `tex`, `mat`, `cam` | `texture`, `matrix`, `camera` |
| `idx`, `cnt`, `num`, `len` | `index`, `count`, `number`, `length` |
| `tmp`, `val`, `obj`, `ptr` | name what it actually holds |
| `buf`, `mgr`, `str`, `attr` | `buffer`, `manager`, `text`, `attribute` |
| `init`, `calc`, `proc` | `Initialize`, `Calculate`, `Process` |

Allowed as-is: `id`, `min`, `max`, `i`/`j`/`k` **only** as loop counters, and
acronyms that are the real name of the thing rather than a shortening of it —
graphics and file-format terms such as `LOD`, `UV`, `VAO`, `VBO`, `EBO`, `GL`,
`RGB`, `AABB`, `glTF`. A new subsystem may bring its own; the test is whether
the expansion is what practitioners actually call it.

**Say the space or the unit whenever the name alone is ambiguous.** A bare
`offset`, `size`, `angle` or `speed` is a question. Answer it in the name —
world space or model space, degrees or radians, pixels or normalized, per
second or per frame — or, when the answer is too long for a name, in a comment
on the declaration.

A function name should let the reader skip the body. Two functions that differ
in behaviour differ in their names, not in a comment attached to one of them.

### Legacy names — do not propagate

Some files predate the current style. As it stands, `Camera.h` uses
`m_camelCase` members and K&R braces, and `Shader.h` uses K&R braces and a
public `GLuint ID`.

The rule is general, not a list: **when a file's local style contradicts this
guide, leave it alone while working nearby and do not copy the pattern into new
code.** If it is worth fixing, fix it in its own `refactor:` commit, never
folded into a feature.

## Formatting

- **Tabs** for indentation, tab width 4.
- **Allman braces**: opening brace on its own line, for types, functions,
  `if`, `for`, `while`, everything.
- Braces always, even for a one-statement body.
- `#pragma once` at the top of every header. No include guards.
- Comments wrap at ~80 columns. Code may run to ~90; past that, wrap.
- Wrapped parameter lists and initializer lists continue on the next line with
  one extra tab.
- Blank line between logical groups of members. Align the names in a
  declaration block when the types have very different widths:

```cpp
	std::unique_ptr<Renderer>      m_Renderer;
	std::vector<Piece>             m_Pieces;
	std::unique_ptr<Animator>      m_Animator;
```

- `public:` before `private:` in new classes, so the reader meets the API
  first. The single reason to invert it is a nested type that the member
  declarations below it depend on; when that happens, it is worth a comment.

### Includes

Headers, in order:

1. `#pragma once`
2. `<glad/glad.h>` if any GL type appears (must precede other GL headers)
3. project headers, relative paths, `"../Core/Shader/Shader.h"`
4. other third party, `<glm/glm.hpp>`
5. standard library, `<vector>`, `<memory>`, `<string>`

Source files: own header first, then the same order.

Forward declare in headers whenever the header only needs the name
(`class Shader;`, `struct ActorContext;`) and include in the `.cpp`. Every
include in a header is coupling.

## What to comment, and what not to

Write a comment when at least one of these is true:

- The reader will ask "why is this here?" or "why this way?"
- There is an invariant, precondition or lifetime rule the code cannot state
- Ordering matters and the reason is not visible
- The obvious approach was rejected for a reason
- The maths or the algorithm is non-trivial — anything a reader could not
  re-derive from the code in front of them, or that came from a paper, a
  reference implementation or a measured value
- A sentinel value carries meaning (`-1`, `nullptr`, empty)

Do **not** comment:

- Trivial getters and setters
- `= default` / `= delete` special members, unless the *reason* for deleting
  is non-obvious (then say the reason, not the fact)
- Anything a good name already says
- Section banners (`// ---- Public ----`)
- Commented-out code — delete it, git has it

Prefer a class-level comment that explains the whole idea once over ten
per-member comments that each explain a fragment.

## Ownership must be visible

Every pointer member gets a comment saying who owns the pointee and what
`nullptr` means:

```cpp
	// Non-owning: the textures live in the ResourceLibrary.
	// nullptr = untextured submesh.
	Texture* texture = nullptr;
```

Rules:

- Prefer a reference over a pointer. A raw pointer member means "optional,
  non-owning", and nothing else.
- `unique_ptr` for owned, polymorphic or non-movable members.
- A type that owns GL names deletes copy **and** move. To store such a type in
  a `std::vector`, hold it through a `unique_ptr` and say so in a comment —
  that indirection is load-bearing, not decoration.
- Deleted copy operations get a one-line reason, in terms of what would break:
  two owners fighting over one GL name, two objects sharing a transform that
  each thinks is its own.

## Engine architecture rules

These are not style; breaking them breaks the engine. Full detail with worked
examples in [architecture.md](architecture.md) — read it before adding a subsystem.

- **Three-way split.** The data/loader layer produces plain CPU data with zero
  GL calls; the renderer owns the GPU buffers and issues the draws; the system
  owns the collection and the per-pass uniforms. Every subsystem that puts
  geometry on screen follows this, and so does the next one.
- **No raw OpenGL** outside `src/Renderer/**` and `src/API/OpenGL/**`. Systems,
  actors, scenes and `World` call renderers, never `gl*` directly. A new
  subsystem that needs GL gets a renderer, not an exception.
- **Contexts, not World.** Pass a context struct. They are built fresh each
  frame, hold references, and deliberately have no default constructor. A new
  dependency means a new reference on the relevant context; nothing downstream
  should learn what `World` is.
- **A render path runs more than once per frame.** Today it is three times
  (water reflection, water refraction, screen), and the count is a property of
  the current passes, not a constant. Never mutate state, advance animation or
  run gameplay inside a render path. That work belongs in `Update`, which runs
  once.
- **No named namespaces.** Use an anonymous namespace in a `.cpp` for
  file-local constants and helpers; nest types inside the class that owns them.
- **New files must be registered by hand.** The Visual Studio project does not
  glob. A `.cpp` with no `ClCompile` entry in `MordoEngine.vcxproj` is never
  compiled, and the failure surfaces as an unresolved external pointing at the
  caller rather than at the missing file. Add the matching entry in
  `MordoEngine.vcxproj.filters` too, so the file shows up where its siblings
  are. See [architecture.md](architecture.md) for both.
- C++20, `/permissive-` conformance on. `noexcept` on operations that cannot
  throw, `const` on everything that does not mutate, `override` on every
  override.

## Platform traps

- **Never name anything `near` or `far`.** They are macros from `windef.h`, so
  a variable or parameter with either name compiles until something pulls in
  `windows.h` first, then fails somewhere unrelated. Same care with `min`,
  `max` and `small`.
- Build the **solution**, not the project file: the include paths use
  `$(SolutionDir)`, so building `MordoEngine.vcxproj` on its own fails to
  resolve them.
- The executable resolves assets relative to the current directory, so it must
  run with the project folder (the one containing `res/`) as its working
  directory.

## Visual Studio setup

Doc comments only reach Quick Info and Parameter Help if Doxygen parsing is
on. Once per machine: **Tools > Options > Text Editor > C/C++ > Code Style >
General > Doc comment style: Doxygen (`///`)**. The same dialog can export the
setting into a project `.editorconfig` so it travels with the repo. Typing
`///` above a declaration then generates the stub with every `@param` already
listed.

## Checklist before calling code done

1. Every name spelled out; no `ctx`, no `pos`, no `tmp`.
2. Every public class and struct has a `///` brief that says what it *is*.
3. Every public constructor with parameters has `@param` for each one that
   isn't self-evident, plus `@throws` if it throws.
4. Every comment survives the test: does it say something the code does not?
5. Every pointer member states owner and meaning of null.
6. Tabs, Allman braces, `#pragma once`, forward declares in the header.
7. No GL outside the renderers. No gameplay in a render path.
8. Every new file added to `MordoEngine.vcxproj` **and** `.vcxproj.filters`.
9. Nothing named `near` or `far`.
10. Nothing commented out, no leftover `TODO` without a name and a reason.

## Related documents

- **[Documenting code](documenting-code.md)** — the full `///` rules, what earns
  a comment and what does not, with worked before/after examples. Read it
  whenever writing header documentation.
- **[Architecture](architecture.md)** — the subsystem split, the context
  structs, the ownership patterns and where new code goes. Read it before
  adding a subsystem.
