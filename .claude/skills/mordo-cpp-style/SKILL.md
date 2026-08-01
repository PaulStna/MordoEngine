---
name: mordo-cpp-style
description: Coding and documentation conventions for MordoEngine (C++20, OpenGL, Visual Studio). Use this whenever writing, editing, reviewing, refactoring, commenting or documenting ANY .h or .cpp file in this engine, and whenever adding a class, a system, a renderer or an actor. Also use it when the user asks to "document this", "clean this up", "review my code", "add comments", or asks how something should be named. Apply it even when the request is only to make code work, since the code still has to land in the house style.
when_to_use: Any C++ work in MordoEngine. Writing a new class or system, editing an existing one, naming a function or variable, deciding what to comment, writing header documentation, reviewing a diff, or answering "how should I write this in this codebase".
---

# MordoEngine C++ style

The conventions live in the repository, as documents meant to be read by people
as much as by this skill. They are the single source of truth; nothing is
repeated here, so there is nothing here that can drift out of date.

**Read `docs/cpp-style.md` before writing or changing any C++ in this repo.**
It carries the naming rules, the formatting, what earns a comment, the
ownership rules, the architecture rules that break the engine when ignored, the
platform traps, and the checklist to run before calling code done.

Then, depending on the task:

| Doing this | Read this as well |
|:--|:--|
| Writing or reviewing header documentation, `///` comments, `@param` | `docs/documenting-code.md` |
| Adding a subsystem, a renderer, an actor, or anything touching OpenGL | `docs/architecture.md` |
| Adding any new `.h` or `.cpp` file at all | `docs/architecture.md`, for the project registration step the build does not do for you |

Paths are relative to the repository root, which is the working directory.

Before writing anything, also read one or two source files next to the one being
touched, and match their shape.
