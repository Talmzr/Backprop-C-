// M2: every operation is gradient-checked: backward()'s gradient for each input
// must agree with the numerical estimate from Gradient_checker.
// Written by Claude at the user's request (2026-10-19).

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <vector>

#include "node.h"

namespace {

constexpr double kStep = 1e-5;

// Agreement within an absolute tolerance (for values near 0) or a relative one.
// The combined matcher is built and used in one statement: Catch2's || keeps
// references to its two parts, so it must not outlive them.
void require_close(double actual, double expected) {
    REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(expected, 1e-7) || Catch::Matchers::WithinRel(expected, 1e-6));
}

// Runs backward on the output, then checks every given input against the checker.
void check_gradients(const std::shared_ptr<Node>& output, std::vector<std::shared_ptr<Node>> inputs) {
    backward(output);
    for (std::shared_ptr<Node>& input : inputs) {
        double from_backward = input->get_gradient();
        double from_checker = Gradient_checker(input, kStep, output);
        INFO("input value " << input->get_value());
        require_close(from_backward, from_checker);
    }
}

}  // namespace

TEST_CASE("M2: subtract and divide, with a node used twice") {
    std::shared_ptr<Node> a = Node_Input(1.5);
    std::shared_ptr<Node> b = Node_Input(-0.5);
    std::shared_ptr<Node> c = Node_Input(2.0);

    std::shared_ptr<Node> l = (a - b) / (a + c);

    require_close(l->get_value(), 2.0 / 3.5);
    check_gradients(l, {a, b, c});
}

TEST_CASE("M2: negate") {
    std::shared_ptr<Node> a = Node_Input(1.5);
    std::shared_ptr<Node> b = Node_Input(-2.0);

    std::shared_ptr<Node> l = -(a * b) + a;

    require_close(l->get_value(), 4.5);
    check_gradients(l, {a, b});
}

TEST_CASE("M2: power, including a negative base") {
    std::shared_ptr<Node> a = Node_Input(1.2);
    std::shared_ptr<Node> b = Node_Input(0.7);
    std::shared_ptr<Node> c = Node_Input(-1.5);

    std::shared_ptr<Node> l = power(a, 3.0) * b + power(c, 2.0);

    require_close(l->get_value(), 1.2 * 1.2 * 1.2 * 0.7 + 2.25);
    check_gradients(l, {a, b, c});
}

TEST_CASE("M2: exp") {
    std::shared_ptr<Node> a = Node_Input(0.5);
    std::shared_ptr<Node> b = Node_Input(1.2);

    std::shared_ptr<Node> l = exp(a * b) + a;

    check_gradients(l, {a, b});
}

TEST_CASE("M2: log, on inputs away from 0") {
    std::shared_ptr<Node> a = Node_Input(2.0);
    std::shared_ptr<Node> b = Node_Input(1.5);

    std::shared_ptr<Node> l = log(a * b) * a;

    check_gradients(l, {a, b});
}

TEST_CASE("M2: tanh") {
    std::shared_ptr<Node> a = Node_Input(0.4);
    std::shared_ptr<Node> b = Node_Input(-0.8);
    std::shared_ptr<Node> c = Node_Input(0.3);

    std::shared_ptr<Node> l = tanh(a * b + c);

    check_gradients(l, {a, b, c});
}

TEST_CASE("M2: relu with a positive input passes the gradient through") {
    std::shared_ptr<Node> a = Node_Input(1.5);
    std::shared_ptr<Node> b = Node_Input(2.0);
    std::shared_ptr<Node> c = Node_Input(-0.7);

    std::shared_ptr<Node> l = relu(a * b) * c;

    require_close(l->get_value(), 3.0 * -0.7);
    check_gradients(l, {a, b, c});
}

TEST_CASE("M2: relu with a negative input blocks the gradient") {
    std::shared_ptr<Node> a = Node_Input(1.5);
    std::shared_ptr<Node> b = Node_Input(-2.0);

    std::shared_ptr<Node> l = relu(a * b);

    require_close(l->get_value(), 0.0);
    check_gradients(l, {a, b});
    require_close(a->get_gradient(), 0.0);
    require_close(b->get_gradient(), 0.0);
}

TEST_CASE("M2: plain numbers on either side give the right values") {
    std::shared_ptr<Node> a = Node_Input(2.0);

    require_close((a + 1.0)->get_value(), 3.0);
    require_close((1.0 + a)->get_value(), 3.0);
    require_close((a - 1.0)->get_value(), 1.0);
    require_close((1.0 - a)->get_value(), -1.0);
    require_close((a * 0.5)->get_value(), 1.0);
    require_close((0.5 * a)->get_value(), 1.0);
    require_close((a / 4.0)->get_value(), 0.5);
    require_close((1.0 / a)->get_value(), 0.5);
}

TEST_CASE("M2: plain numbers on either side give the right gradients") {
    std::shared_ptr<Node> a = Node_Input(2.0);
    std::shared_ptr<Node> b = Node_Input(0.5);

    std::shared_ptr<Node> l = (1.0 - a) * (a / 4.0) + 3.0 / (b + 1.0) - (2.0 * b - 0.5) + (a * 0.25 + 1.5);

    check_gradients(l, {a, b});
}

TEST_CASE("M2: an intermediate node used several times through the new operations") {
    std::shared_ptr<Node> a = Node_Input(0.6);
    std::shared_ptr<Node> b = Node_Input(1.1);

    std::shared_ptr<Node> d = exp(a) - b;
    std::shared_ptr<Node> l = d * d / (a + 2.0) + tanh(d) - power(d, 3.0) + log(b + d * d);

    check_gradients(l, {a, b});
}
