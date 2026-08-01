---
name: mordo-changelog
description: Writes MordoEngine changelog entries in docs/changelog.md and the matching Conventional Commit message. Use this whenever the user asks to update, write, add to or fix the changelog, asks "what should I put in the changelog", asks for a commit message, or finishes a chunk of work and wants it recorded. Also use it after any feature or refactor lands, since every day of work in this project gets a dated entry.
when_to_use: Updating docs/changelog.md, writing a changelog entry for today's work, summarizing a diff or a set of commits into changelog bullets, or writing the commit message that goes with them.
argument-hint: [optional: what changed, or a date]
---

# MordoEngine changelog

## Live repository state

- Recent commits: !`git log --oneline -12 2>/dev/null || echo "(not a git repo)"`
- Working tree: !`git status --short 2>/dev/null | head -40`
- Diff summary: !`git diff --stat HEAD 2>/dev/null | tail -30`
- Today: !`date +%Y-%m-%d`
- Tail of the current changelog: !`tail -n 15 docs/changelog.md 2>/dev/null || echo "(docs/changelog.md not found from here)"`

If the working tree above is empty, the work is already committed — read the
commits instead. Never write an entry from the conversation alone; read the
actual diff first with `git diff`, `git diff HEAD`, or `git show <sha>`.

## The file is CRLF with no trailing newline — read this before writing

`docs/changelog.md` uses **CRLF** line endings throughout and **does not end in
a newline**. Both facts matter more than they look:

- Anything that reads the whole file and writes it back — Python
  `read_text()` / `write_text()`, most editors' "normalize line endings",
  any round-trip through a text-mode `open()` — rewrites all CRLF as LF and
  turns a two-line addition into a diff of the entire file.
- Because there is no trailing newline, an append must supply its **own**
  leading `\r\n`, or the first new bullet fuses onto the last existing line.

Append in binary and write the separators explicitly:

```python
from pathlib import Path

bullets = [
    "Added ...",
    "Refactored ...",
]

path = Path('docs/changelog.md')
with path.open('ab') as file:
    for bullet in bullets:
        file.write(b'\r\n- ' + bullet.encode('utf-8'))
```

A new day heading is written the same way, as `b'\r\n\r\n## 2026-08-01'`
followed by its bullets.

Verify afterwards. Correct output has zero bare LF and no trailing newline,
and the diff touches only the lines added plus the previous last line, which
git reports as modified because it had no terminator:

```python
data = Path('docs/changelog.md').read_bytes()
print('CRLF:', data.count(b'\r\n'), 'bare LF:', data.count(b'\n') - data.count(b'\r\n'))
print('ends with newline:', data.endswith(b'\n'))
```

Then `git diff --stat docs/changelog.md`. A diff of hundreds of lines means the
endings were destroyed — revert it and append again in binary.

## Format

`docs/changelog.md` is a dated engineering journal. It runs **oldest first**;
new entries are **appended at the end of the file**.

```markdown
## 2026-07-31
- Added an actor layer under src/Actor: Actor owns the transform, ActorSystem
  updates every actor once per frame, and ActorContext hands it the terrain,
  the camera and the system itself.
- Made ModelSystem hold its models by unique_ptr, so the reference Add returns
  survives later calls and an actor can keep pointing at its model.
```

Rules:

- `## YYYY-MM-DD` heading, one per day of work. If today's heading already
  exists, append bullets under it rather than opening a second one.
- No version numbers. No `### Added` / `### Fixed` sections. No Keep a
  Changelog categories. This project does not use them.
- One `-` bullet per meaningful change, not per file touched.
- English, US spelling, past tense.
- Blank line between day sections.

## Verbs

Every bullet opens with a past-tense verb that names the kind of change. The
ones already in use, in rough order of frequency:

`Added` · `Implemented` · `Refactored` · `Updated` · `Removed` · `Made` ·
`Split` · `Replaced` · `Extracted` · `Changed` · `Moved` · `Dropped` ·
`Cleaned` · `Nested` · `Registered` · `Vendored` · `Tracked` · `Gave` ·
`Reworked` · `Reorganized` · `Reduced` · `Preserved` · `Propagated`

Pick the one that is actually true. `Extracted` and `Split` say more than
`Refactored`; `Made` and `Gave` fit a change of behaviour on something that
already existed. The list is what has been used so far, not a closed set — a
change that is genuinely something else gets the verb that is genuinely true.

## Voice

The early entries in this file are one-line notes. The recent ones are the
target: **one to three sentences that say what changed and why**, naming real
symbols, and stating the constraint that forced the design when there was one.

Write like this:

> Extracted a Model class out of ModelSystem: a Model owns one model's
> buffers, the draw call and texture of each submesh and its place in the
> world, while ModelSystem became the director of a std::vector<Model>. Model
> holds its renderer through a unique_ptr because ModelRenderer is neither
> copyable nor movable, and that indirection is the only thing making Model
> storable in a vector at all.

Not like this:

> Refactored the model system.

The example is about the shape, not the subsystem — an entry about terrain,
audio, UI or anything added later is written to the same standard. What
separates the two:

- **Real names.** Whatever the reader would need to `grep` for: the classes,
  the functions, the paths, the third-party flags. Name them.
- **The why, when there is one.** Say what forced the shape: a type that is
  not movable, a pass that runs several times a frame, a library that throws on
  duplicates, two library flags that cannot coexist.
- **The rejected alternative, when it matters.** "Deliberately without
  aiProcess_FlipUVs, since Texture already flips through stb and applying both
  cancels out."
- **Honest limits.** If something is unoptimized, partial, or a test, say so:
  "(still unoptimized)", "Morph target animation is documented as not
  implemented".

Keep it factual. No marketing, no "improved developer experience", no emoji,
no exclamation marks.

## Granularity

One bullet covers one idea, even if it spans six files. Split into separate
bullets when the changes are independently understandable — a new subsystem,
the shader work it needed, and the resource registration it needed are three
bullets, not one and not twelve.

Do not write a bullet for: formatting-only changes, a rename with no
behavioural effect, or anything invisible to someone reading the code later.
Do write one for: a vendored dependency, a project-file change, a moved
responsibility, a deleted class, a documented limitation.

## Procedure

1. Read the actual changes: `git diff HEAD`, or `git show` for each commit
   since the last changelog date.
2. Group them by subsystem — the folder under `src/` each change belongs to —
   plus one group for build and project setup. Group by subsystem, never by
   file.
3. Order the bullets so the reader can follow: the main change first, then
   what it required, then cleanup.
4. For each, ask "would this let me find the code and understand why?" If not,
   add the symbol name or the reason.
5. Check whether today's `## YYYY-MM-DD` heading already exists before adding
   one.
6. Append to the end of `docs/changelog.md`, **in binary, preserving CRLF**.
7. Verify the endings and the diff size, as above.
8. Show the user the entry before writing it if the diff was large or
   ambiguous.

## Commit messages

The same work gets a Conventional Commit. Format:

```
type(scope): lowercase imperative summary
```

Types in use: `feat`, `refactor`. Add `fix`, `docs`, `chore`, `build` when
they genuinely apply.

**Scope is the subsystem the change belongs to, lowercase — normally the
folder under `src/`.** The history so far has used `actor`, `models`,
`physics`, `render`, `renderer`, `terrain` and `engine`. That is a record, not
a menu: a change in a subsystem with no scope yet introduces its own, named
after its folder, rather than being forced into one of these. Omit the scope
entirely when the change is engine-wide.

Real examples from this repo:

```
feat(actor): introduce the Actor abstraction behind every object in the scene
feat(models): add Assimp model loading with skeletal animation
refactor(render): move the underwater post-process into WaterSystem and remove GL from World
refactor(terrain): drop the terrain namespace and nest its types inside Terrain
```

Summary line: imperative mood ("add", not "added" — the opposite of the
changelog), no trailing period, under ~75 characters. A body is optional;
when the changelog bullet already carries the reasoning, the summary alone is
enough, and that is what this repo has done every time so far.

## Checklist

1. Read the real diff, not the conversation.
2. Correct date heading, appended at the end of the file, no duplicate heading.
3. Appended in binary; CRLF intact, no trailing newline, diff is small.
4. Every bullet starts with a past-tense verb.
5. Every bullet names at least one real symbol, file or path.
6. Every non-obvious design decision states its reason.
7. No version numbers, no categories, no emoji.
8. Commit message in Conventional Commit form, imperative, correct scope.
