# Backprop: an autograd engine from scratch in C++

[![CI](https://github.com/Talmzr/Backprop-C-/actions/workflows/ci.yml/badge.svg)](https://github.com/Talmzr/Backprop-C-/actions/workflows/ci.yml)

A small reverse-mode automatic differentiation engine, written from scratch in C++20 with no autodiff or machine-learning libraries. Every number in a calculation becomes a node in a graph; calling `backward()` on the result computes the gradient of that result with respect to every node, by applying the chain rule backwards through the graph.

The goal of the project is to understand backpropagation by building it: first a scalar engine, then a small neural network trained on XOR, then a tensor engine trained on MNIST.

## What it does today

- **Operations:** `+`, `-`, `*`, `/`, negation, `power` (constant exponent), `exp`, `log`, `tanh`, `relu`, and plain numbers mixed in on either side (`a + 2.0`, `1.0 / a`).
- **Backward pass:** a depth-first search builds a topological order of the graph; gradients are then propagated from the output back to the inputs, accumulating (`+=`) when a node is used more than once.
- **Gradient checker:** a central-difference estimate, (L(x + h) − L(x − h)) / 2h, computed by replaying the graph with one input nudged, used to verify every operation's backward rule.
- **Memory safety:** nodes are shared through `std::shared_ptr`, and pointers only go from a node to its inputs, so a graph is freed automatically when its output goes out of scope.

## Example

```cpp
#include "node.h"

int main() {
    auto a = Node_Input(2.0);
    auto b = Node_Input(-3.0);
    auto c = Node_Input(10.0);

    auto L = tanh(a * b + c) * 2.0 - power(a, 2.0);

    backward(L);

    std::cout << "dL/da (backward): " << a->get_gradient() << "\n";
    std::cout << "dL/da (numerical): " << Gradient_checker(a, 1e-5, L) << "\n";

    graph(L);   // print every node: value, operation, inputs, gradient
}
```

Output (node addresses vary between runs):

```
dL/da (backward): -4.00805
dL/da (numerical): -4.00805
 Node: 000002832F10C210 Value: 2 op: None Parents:  Gradient -4.00805
 Node: 000002832F10A470 Value: -3 op: None Parents:  Gradient 0.0053638
 Node: 000002832F118080 Value: -6 op: multiply Parents: 000002832F10C210 000002832F10A470  Gradient 0.0026819
 ...
 Node: 000002832F129EC0 Value: -2.00134 op: subtract Parents: 000002832F129640 000002832F1112D0  Gradient 1
```

The gradient computed by `backward()` and the numerical estimate agree.

## Verification

15 tests (Catch2) check the engine:

- gradients of hand-worked expressions, including a node used more than once and an intermediate node reused in several places, against values computed by hand;
- every operation's gradient against the numerical gradient checker, including all eight ways of mixing nodes with plain numbers and both branches of `relu`, plus checks of the values the mixing operators produce.

They run on every push through GitHub Actions:

- **Linux, GCC and Clang,** under AddressSanitizer, UndefinedBehaviorSanitizer and LeakSanitizer;
- **Windows, MSVC,** in a debug build where any memory still allocated when the tests end fails the run.

## Building and testing

Catch2 is downloaded automatically at configure time. Run the commands in the `cmake/` folder.

**Linux:** a C++20 compiler (GCC 12+ or Clang 16+), CMake 3.25 or later, and Ninja.

```
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --preset linux-debug
```

**Windows:** Visual Studio 2022 or later (the "Desktop development with C++" workload, including the AddressSanitizer component), which also provides CMake and Ninja. From a Visual Studio developer command prompt:

```
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

The program itself is `build/<preset>/backprop` (`backprop.exe` on Windows).

| Windows preset | Linux preset | Use |
|---|---|---|
| `debug` | `linux-debug` | Development and tests; on Windows a leak reported by the debug heap fails the test |
| `asan` | `linux-asan` | Tests under AddressSanitizer; on Linux also UndefinedBehaviorSanitizer and LeakSanitizer |
| `release` | `linux-release` | Optimized build for training runs |

The Windows presets target ARM64 when used from VS Code. From a developer command prompt they build for whatever architecture the prompt is set up for; to use them on x64 from VS Code, change `arm64` to `x64` in `cmake/CMakePresets.json` (the `architecture` and `toolset` fields).

## Project layout

```
cmake/     CMakeLists.txt and CMakePresets.json
src/       the engine (node.h, node.cpp) and Main.cpp
tests/     Catch2 tests: backward pass, gradient checker, every operation
docs/      progress log, notes, diagrams
.github/   CI workflow
```

## Roadmap

| Milestone | Status |
|---|---|
| Computation graph with `+` and `*` | done |
| Backward pass, topological sort, gradient checker | done |
| Full set of scalar operations | done |
| Neuron, layer and MLP; MSE loss; gradient descent; train XOR | in progress |
| Tensor engine backed by Eigen; same network on tensors | planned |
| Softmax and cross-entropy; train on MNIST (~97% test accuracy) | planned |
