# Progress

## Current milestone

**M3 — Neuron, layer, MLP; XOR.** Started 2026-10-19.

- **Where the user started:** pictured a neuron as `a*w + b` growing with the layers; no picture yet of layers or the network in node terms; knew MSE and gradient descent in principle, not their implementation. Claude explained neuron, layer, MLP, MSE, the training step, gradient reset, graph lifetime per step, initialization and symmetry, settable weights.
- **User's design (revised 2026-10-22):** each neuron holds its bias and the weights of its *incoming* connections, from the previous layer (first plan was outgoing weights). Its output is currently built from an `inputs` vector of (previous output, weight) pairs. The neuron does not keep the previous layer's values; the graph does.
- **Code so far (2026-10-22):** `Node::reset_gradient()` in `node.cpp`; the neuron code moved by the user into `src/neuron.h` / `src/neuron.cpp` (added to the `engine` library in CMake): `class Neuron` (fields: `weights`, `value`, `bias`); `inputs_sum(inputs, weights)` builds the weighted sum as a chain of node `*` and `+` (index bug fixed with `back()`) and collects the weight pointers; `neuron_creator(inputs)` draws a random bias from a `static std::mt19937(42)`, builds `tanh(sum + bias)` and returns a `shared_ptr<Neuron>`. Earlier versions cut the graph (`double` sums, `Z` as a fresh input); fixed.
- **Decided 2026-10-23:** fresh graph every forward pass (see Decisions log). `neuron_recompute`, which wrote plain numbers into the existing output node, is to be replaced by computing the output from nodes each time.
- **Raised, not yet decided:** `neuron_creator` both creates a neuron (new bias) and computes its output; training needs creating once and computing every step. Who creates each neuron's incoming weight nodes, and their starting values (random, and settable for M4); right now they arrive inside `inputs` from the caller. The output neuron's activation.
- **Not started:** layer, MLP, MSE loss, the training loop (forward, reset, backward, update), the XOR run in Release, tests (gradient check of a small network's loss, loss decreasing, many steps for the leak check).

## Done

- **M2 — More operations** (finished 2026-10-19, commit `182000d`, pushed). All operations written by the user:
  - **New operations:** subtract, divide, negate (unary `operator-`), power (now the named function `power(node, exponent)`, stored as a second parent: an input node that never receives a gradient), `exp`, `log`, `tanh`, `relu`. Each has an enum member, a creating function, a case in `graph`, a backward rule `gradient_p_*` with its `if` in `backward`, and a branch in `replay_network`, which now uses explicit first and second parents.
  - **Mixing with numbers:** eight overloads for `+ − * /`, number on either side, each wrapping the number with `Node_Input`. `1.0 - a` and `1.0 / a` were briefly reversed; fixed.
  - **Bugs found and fixed along the way:** copy-paste of the wrong `Operation` in `log` and `relu`; the replay accumulating subtract and divide over the parents; relu's replay comparing the pointer, not the value, and always ending at 0; negate passing a single pointer where the constructor wants a vector.
  - **Explained on request:** what `exp`, `tanh`, `relu` are and why the project needs them; tanh versus sigmoid and the other activations; why a network without activations collapses to a straight line (and why that can't do XOR); why the checker returns 0 for computed nodes such as `d`; why `h = 1e-8` gives visible rounding error when the output is large (use about 1e-5).
  - **Tests:** `tests/m2_operations_test.cpp`, written by Claude at the user's request: every operation and all eight number-mixing overloads gradient-checked (h = 1e-5, absolute 1e-7 or relative 1e-6), both relu branches, value checks for the mixing operators, and an intermediate node used several times through the new operations. All 17 tests pass in Debug (no leak reports) and under ASan, run by Claude on 2026-10-19. A first run crashed because of a bug in Claude's test helper (a Catch2 combined matcher returned from a function keeps references to destroyed parts); fixed, not an engine problem.
- **M1b — Backward** (finished 2026-10-16, commit `d779a3b`, pushed). All engine code is in `src/node.h` (declarations) and `src/node.cpp` (definitions), written by the user; `src/Main.cpp` holds only `main`.
  - `add_to_gradient(double)` (first named `set_gradient`) adds to the gradient (`+=`); `set_value(double)` replaces the value (added for the checker).
  - Backward rules chosen by operation, not stored closures: `gradient_p_addition` passes the node's gradient to each parent; `gradient_p_multiply` adds g·vⱼ for every parent, then subtracts g·vₙ for the parent itself.
  - `topological(node, seen, order)`: DFS with an `std::unordered_set` of visited nodes, appending each node after its parents; marks a node seen when its visit ends.
  - `backward(output)`: builds the order, seeds the output with 1, applies the rule to each node from the back of the order.
  - `replay_network(output)`: recomputes every value in place from the inputs; `Gradient_checker(input, h, output)`: central difference using the replay, restores the original value exactly and replays once more. `get_inputs` collects the input nodes from an order (currently unused).
  - Tests, all written by the user: `tests/backward_test.cpp` (the M1a expression; every input used twice; an intermediate node `d` used twice, including `d`'s own gradient) and `tests/Gradient_checker_test.cpp` (the checker against the reused-intermediate expression, tolerance 1e-8, h = 1e-4). Expected values hand-worked by the user, except the third test's, which Claude derived at the user's request.
  - Verified: all six tests pass in Debug (so no leak reports) and under ASan (run by Claude at the user's request, 2026-10-16: 6/6, no AddressSanitizer errors).
- **Where the user started in M1b, and what worked:** pictured backprop by layers; had never done DFS. Wanted the theory without examples or step-by-step, then to work out the code themselves; asked for worked examples only when stuck. Guiding questions ("what must be true about the node's gradient when you process it?") worked well. Watch for: range-`for` versus index loops, unsigned counters counting down, unsaved files, and VS Code's build target being `backprop` instead of `all`.

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

0. (2026-11-09) M3 work resumes on branch `m3-neuron`, brought up to date with `main` and with `neuron.h`/`neuron.cpp` restored from `dd38082`; Claude re-adds `neuron.cpp` to CMake there. CI runs on that branch too and will fail until the neuron code compiles; the README badge shows `main` only.
1. The user separates creating a neuron from computing its output, and decides where the weights are created.
2. Then the layer and the network, the loss, and the training loop.

## Review of M1a (2026-10-14, requested by the user)

Claude rebuilt `backprop` in `debug` and `asan` with the user's permission: no compiler warnings at `/W4`, correct output in both, no ASan errors. M1a is pushed (commit `5be74f1`). No bugs found. Points given to the user:

- **Fixed by the user the same day (all four):** `get_parents()` now returns `const std::vector<...>&`; `Graph` and its two loop variables take `const std::shared_ptr<Node>&`; the constructor sets all four fields in its initializer list, in declaration order (the user also reordered the fields by size: vector, two doubles, enum); `using namespace std;` is removed and `std::` written out everywhere. Not rebuilt by Claude after these changes.
- **Known limits, fine for now:** `Graph` prints a node once per path that reaches it (matters in M1b); the `switch` in `Graph` prints an empty operation for any enum member added later (matters in M2); nothing checks for a null pointer being passed to the operators or `Graph`.
- **Style, left to the user:** mixed naming (`PrevLayer`, `Gradient`, `nodeop`, `ValArithm`, `Node_child`), uneven indentation, the unnecessary `if` in `Graph`.
- **Not covered by any tool yet:** there are no tests for `Node`, and the leak report only runs inside the test program.

## Open problems / questions

- **Neither memory checker has been seen catching anything** (see Decisions log, 2026-10-12). ARM64 ASan ran the program (2026-10-14) and all tests (2026-10-16) cleanly. The leak gate is part of "done" from M1b on.
- **Power** was declared as `operator^`, which binds more loosely than `+` and `*`; on 2026-10-19 the user replaced it with a named function `power(node, exponent)`. Resolved. Note: the exponent is an input node, so `get_inputs` lists it too; checking it numerically would disagree with `backward`, which gives it 0 by design.
- **For M2:** the central `switch` design means each new operation touches the enum, `ValArithm`, the text `switch` in `graph`, `replay_network`, an operator function, and a backward rule plus its `if` in `backward`. Claude can switch on a compiler warning for a `switch` that misses an enum member; offer it at the start of M2.
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
- 2026-11-07: The build is cross-platform: MSVC stays the compiler for local work; CMake now also supports GCC and Clang (warnings `-Wall -Wextra`; the asan option adds UBSan and, on Linux, LeakSanitizer), with `linux-*` presets. GitHub Actions (`.github/workflows/ci.yml`) runs the tests on every push: Linux GCC and Clang under ASan + UBSan + LSan, and Windows MSVC x64 in Debug with the CRT leak check. The MSVC-only `FATAL_ERROR` is gone. Reason: the repository should build for reviewers on any platform. User's request; written by Claude (build configuration).
- 2026-11-07: First CI run (commit `717a52a`) passed: 17/17 tests on Linux GCC and Clang under ASan + UBSan + LSan, and on Windows MSVC x64 Debug. Warnings left for the user: `-Wswitch` on `ValArithm`'s switch (node.cpp line 24) and `-Wrange-loop-construct` on the copying loop variable in `get_inputs` (node.cpp line 344).
- 2026-11-07: `main` must always build. The unfinished M3 neuron/layer code (commit `dd38082`) moves to branch `m3-neuron`; on `main` it is removed and `neuron.cpp` is taken out of CMake. Re-add it to CMake when M3 work continues on the branch. Claude wrote `README.md` at the user's request (not engine code); its example was compiled and run against the engine.
- 2026-10-23: Training builds a fresh graph on every forward pass (dynamic graph): weights and biases are persistent nodes owned by the neurons; every other node is rebuilt each step and freed with the step's loss. Rejected: building the graph once and replaying it each step (static graph). Reason (user's decision, after comparing both): simpler, flexible in batch size and data, keeps the gradient checker's replay independent of training; its allocation cost largely disappears with M4's tensors.
- 2026-10-22: Each neuron holds the weights of its incoming connections (from the previous layer), not its outgoing ones. User's design change.
- 2026-10-22: The random generator for starting values is a `static` local in `neuron_creator` (seed 42), so each neuron draws different numbers. Known limits, accepted by the user for now: the sequence is shared by every network in a program, and the seed can only be changed in the code. Revisit only if it becomes a problem (for example by moving the generator into the network).
- 2026-10-19: Claude may write tests when the user asks (user's decision: writing the M2 tests had no value for them). `CLAUDE.md` updated. First case: `tests/m2_operations_test.cpp`.
- 2026-10-16: Backward rules are chosen from each node's operation (`if`s calling one function per operation), not stored in each node as a closure. User's design; `CLAUDE.md` core decisions updated to match.
- 2026-10-16: The M1a expression is not separately checked with the gradient checker. User's decision: the reused-node expression the checker test uses contains the same operations and shape.
- 2026-10-16: Node values became settable (`set_value`), so the checker can nudge inputs and replay the graph in place. Note for M3: weight updates can use the same setter.
- 2026-10-14: The user asked Claude to move their existing code from `Main.cpp` into `node.h` ("there is no value in it"), after Claude had said its rules keep it out of their source files. Done verbatim, no new code. Reason: purely mechanical. Claude still does not write or change the user's code itself.
- 2026-10-14: Engine code is shared through an `engine` CMake target (INTERFACE, include directory `src/`), linked by both `backprop` and `tests`. Reason: tests need `#include "node.h"`; one target keeps the include path and later sources in one place.
- 2026-10-12: M0 closed with tests run from the command line rather than VS Code's Testing panel. Reason: the user wanted to move on; the panel gets tried with the first real tests.
