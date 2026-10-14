# Progress

## Current milestone

**M1b — Backward.** Not started. M1a was finished on 2026-10-14; the user has not yet said to begin M1b.

When it starts: first ask what the user already knows about backward passes and the chain rule on a graph (no derivative quiz), then fill in gaps and state "done".

## Done

- **M1a — Build the graph** (finished 2026-10-14, commit `5be74f1`, pushed). Everything is in `src/Main.cpp`, written by the user:
  - `enum class Operation { add, multiply, None }`.
  - `class Node`: private fields `value`, `PrevLayer` (a `const` vector of `std::shared_ptr<Node>`, the parents), `nodeop` (`const`), `Gradient`; one constructor taking operation, value and parents; four read functions (`get_value`, `get_gradient`, `get_operation`, `get_parents`).
  - `ValArithm(op, parent1, parent2)`: central arithmetic with a `switch`; throws `std::logic_error` for any other operation.
  - `Node_child(op, parent1, parent2)`: builds a result node with `make_shared` and returns the shared pointer. `Node_Input(value)`: builds an input node.
  - `operator+` and `operator*` on `std::shared_ptr<Node>`, each one line calling `Node_child`.
  - `Graph(node)`: prints parents first, then the node's address, value, operation and parent addresses.
  - Verified on `(a * b + c) * f` with 2, -3, 10, -2: seven nodes, values and operations right, every parent address matching. The user reports the single-expression form works too.
- **User's design decisions in M1a:** a variable like `a` is the shared pointer itself, no wrapper type; parents are an ordered vector, not a set; all fields private with read functions; parents and operation never change after creation; arithmetic lives outside the constructor, in one central function.
- **Where the user started in M1a:** pictured a node as a neuron; knew printing, loops and `if` in C++ and nothing else. Strong C. What worked: introducing each C++ feature as the answer to something they already do in C, with a small example on an unrelated type, and letting them fix their own code from a description of the problem.
- **M0 — Setup** (finished 2026-10-12, commit `24d1fbb`, pushed):
  - `cmake/CMakeLists.txt` and `cmake/CMakePresets.json` with three presets: `debug`, `asan`, `release`. Each builds into `build/<preset>`.
  - Catch2 v3.16.1 via `FetchContent`; one example test in `tests/example_test.cpp`.
  - Two targets: `backprop` (from `src/Main.cpp`) and `tests`.
  - Debug: Catch2's CRT leak report is on, and a test whose output contains a leak report fails.
  - ASan: `/fsanitize=address`, without `/RTC1` and incremental linking; the runtime DLL is copied beside the executables.
  - The example test passes in all three presets from the command line. The user builds and runs the `debug` preset from VS Code.
- Before M0: `CLAUDE.md` reviewed and revised with the user (2026-10-11); project moved to `C:\dev\Backprop`; Git repo connected to GitHub.

## Next step

1. User commits and pushes the review fixes.
2. Start M1b when the user says so. Its first practical step: `Node` and its functions move out of `Main.cpp` into their own header so tests can include them. The user picks the file name; Claude adds it to CMake.

## Review of M1a (2026-10-14, requested by the user)

Claude rebuilt `backprop` in `debug` and `asan` with the user's permission: no compiler warnings at `/W4`, correct output in both, no ASan errors. M1a is pushed (commit `5be74f1`). No bugs found. Points given to the user:

- **Fixed by the user the same day (all four):** `get_parents()` now returns `const std::vector<...>&`; `Graph` and its two loop variables take `const std::shared_ptr<Node>&`; the constructor sets all four fields in its initializer list, in declaration order (the user also reordered the fields by size: vector, two doubles, enum); `using namespace std;` is removed and `std::` written out everywhere. Not rebuilt by Claude after these changes.
- **Known limits, fine for now:** `Graph` prints a node once per path that reaches it (matters in M1b); the `switch` in `Graph` prints an empty operation for any enum member added later (matters in M2); nothing checks for a null pointer being passed to the operators or `Graph`.
- **Style, left to the user:** mixed naming (`PrevLayer`, `Gradient`, `nodeop`, `ValArithm`, `Node_child`), uneven indentation, the unnecessary `if` in `Graph`.
- **Not covered by any tool yet:** there are no tests for `Node`, and the leak report only runs inside the test program.

## Open problems / questions

- **With private fields, backward needs a way to change another node's gradient.** Mentioned to the user in one line during M1a; to be designed by them in M1b.
- **VS Code's Testing panel is still untried**, and `backprop` and `tests` share no code yet.
- **Neither memory checker has been seen catching anything** (see Decisions log, 2026-10-12). ARM64 ASan did run the program cleanly on 2026-10-14. The leak gate becomes part of "done" from M1b on.
- **For M2:** the central `switch` design means each new operation touches the enum, `ValArithm`, the text `switch` in `Graph`, and an operator function. Claude can switch on a compiler warning for a `switch` that misses an enum member; offer it at the start of M2.
- `docs/node-graph.mmd` is a Mermaid diagram of the example graph, made at the user's request.

## Folder layout

Chosen by the user on 2026-10-12.

- `cmake/` — `CMakeLists.txt` and `CMakePresets.json`. This is the CMake source directory, so `cmake --preset`, `cmake --build --preset` and `ctest --preset` are run from inside it. VS Code is pointed at it with `cmake.sourceDirectory`.
- `docs/` — `PROGRESS.md` and `NOTES.md`.
- `src/` — the user's code (`Main.cpp` so far).
- `tests/` — test files.
- `build/` — build output, ignored by Git.
- `CLAUDE.md` stays at the root, because Claude Code only loads it from there.

## Machine

- Windows 11 on ARM64 (Snapdragon X Plus). Presets build native ARM64.
- Visual Studio Community 2026 (18.3, Insiders), MSVC 14.50. CMake 4.1.2 and Ninja 1.12.1 ship inside it; neither is on `PATH`.
- Command-line builds need the developer environment first: `VsDevCmd.bat -arch=arm64 -host_arch=arm64` (in `Common7\Tools` of the Visual Studio install). Running `ctest.exe` from the Visual Studio CMake folder works without it.
- VS Code has CMake Tools and the C/C++ extension. The C/C++ Runner extension is disabled for this workspace.

## Decisions log

- 2026-10-11: Project lives at `C:\dev\Backprop`, outside OneDrive; GitHub is the only backup. Reason: OneDrive is full, and sync would lock build files.
- 2026-10-11: The Git remote uses the personal GitHub account Talmzr (`https://Talmzr@github.com/Talmzr/Backprop-C-.git`). Reason: this machine was signed in as Talmzrahi, which cannot push to the repo.
- 2026-10-11: When `CLAUDE.md` and this file disagree, Claude tells the user what each says and asks.
- 2026-10-11: Decisions are raised when needed, not in advance (user's preference).
- 2026-10-12: MSVC comes from Visual Studio Community 2026, not the standalone Build Tools `CLAUDE.md` mentions. Same compiler; it was already installed.
- 2026-10-12: The deliberate leak and out-of-bounds checks are dropped from M0 (removed from `CLAUDE.md` too). Reason: the user knows the tools and doesn't want to spend time proving them.
- 2026-10-12: Ninja with one build folder per preset, instead of a Visual Studio multi-configuration project. Reason: ASan needs different flags for every target including Catch2, which is simplest as a separate build tree.
- 2026-10-12: ASan is the Debug build type plus a `BACKPROP_ASAN` option, not a custom build type. Reason: keeps the standard Debug flags and debug CRT, minus `/RTC1`.
- 2026-10-12: `/W4` on the project's own targets, warnings not treated as errors.
- 2026-10-12: `.vscode` stays tracked in Git. Reason: it now holds project settings with no machine-specific paths.
- 2026-10-12: Folder layout split into `cmake/`, `docs/`, `src/` (user's choice, see "Folder layout"). `CLAUDE.md` is the one exception, kept at the root so it still loads.
- 2026-10-12: M0 closed with tests run from the command line rather than VS Code's Testing panel. Reason: the user wanted to move on; the panel gets tried with the first real tests.
