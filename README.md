# Backprop: an autograd engine from scratch in C++

A small reverse-mode automatic differentiation engine, written from scratch in C++20 with no autodiff or machine-learning libraries. Every number in a calculation becomes a node in a graph; calling `backward()` on the result computes the gradient of that result with respect to every node, by applying the chain rule backwards through the graph.

The goal of the project is to understand backpropagation by building it: first a scalar engine, then a small neural network trained on XOR, then a tensor engine trained on MNIST.

## What it does today

- **Operations:** `+`, `-`, `*`, `/`, negation, `power` (constant exponent), `exp`, `log`, `tanh`, `relu`, and plain numbers mixed in on either side (`a + 2.0`, `1.0 / a`).
- **Backward pass:** a depth-first search builds a topological order of the graph; gradients are then propagated from the output back to the inputs, accumulating (`+=`) when a node is used more than once.
- **Gradient checker:** a central-difference estimate, (L(x + h) − L(x − h)) / 2h, computed by replaying the graph with one input nudged, used to verify every operation's backward rule.
- **Memory safety:** nodes are shared through `std::shared_ptr`, and pointers only go from a node to its inputs, so a graph is freed automatically when its output goes out of scope. The test suite runs under the MSVC debug heap's leak report and under AddressSanitizer.

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

## Building and testing

Requirements: Windows with Visual Studio 2022 or later (the "Desktop development with C++" workload, including the AddressSanitizer component), CMake 3.25 or later, and Ninja (both ship with Visual Studio). Catch2 is downloaded automatically at configure time.

From a Visual Studio developer command prompt, in the `cmake/` folder:

```
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

The program itself is `build/debug/backprop.exe`.

There are three presets:

| Preset | Use |
|---|---|
| `debug` | Development and tests; a leak reported by the debug heap fails the test |
| `asan` | Tests under AddressSanitizer (out-of-bounds access, use-after-free) |
| `release` | Optimized build for training runs |

The presets are set up for Windows on ARM64. On an x64 machine, change `arm64` to `x64` in `cmake/CMakePresets.json` (the `architecture` and `toolset` fields) when building from VS Code.

## Project layout

```
cmake/   CMakeLists.txt and CMakePresets.json
src/     the engine (node.h, node.cpp) and Main.cpp
tests/   Catch2 tests: backward pass, gradient checker, every operation
docs/    progress log, notes, diagrams
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
