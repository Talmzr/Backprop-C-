#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "node.h"


TEST_CASE("Gradients for m1a expression"){

    std::shared_ptr<Node> a = Node_Input(2);
    std::shared_ptr<Node> b = Node_Input(-3);
    std::shared_ptr<Node> c = Node_Input(10);
    std::shared_ptr<Node> f = Node_Input(-2);

    std::shared_ptr<Node> l = (a*b + c)*f;

    backward(l);

    REQUIRE_THAT(a->get_gradient(), Catch::Matchers::WithinAbs(6.0, 1e-12));
    REQUIRE_THAT(b->get_gradient(), Catch::Matchers::WithinAbs(-4.0, 1e-12));
    REQUIRE_THAT(c->get_gradient(), Catch::Matchers::WithinAbs(-2.0, 1e-12));
    REQUIRE_THAT(f->get_gradient(), Catch::Matchers::WithinAbs(4.0, 1e-12));

}

TEST_CASE("Gradient for node used twice")
{

    std::shared_ptr<Node> a = Node_Input(2);
    std::shared_ptr<Node> b = Node_Input(-3);
    std::shared_ptr<Node> c = Node_Input(5);
    std::shared_ptr<Node> f = Node_Input(-2);

    std::shared_ptr<Node> l = (a*b + c)*f  + (a*c + f)*b;    

    backward(l);

    REQUIRE_THAT(a->get_gradient(), Catch::Matchers::WithinAbs(-9.0, 1e-12));
    REQUIRE_THAT(b->get_gradient(), Catch::Matchers::WithinAbs(4.0, 1e-12));
    REQUIRE_THAT(c->get_gradient(), Catch::Matchers::WithinAbs(-8.0, 1e-12));
    REQUIRE_THAT(f->get_gradient(), Catch::Matchers::WithinAbs(-4.0, 1e-12));

}

TEST_CASE("Gradient Intermidiate node used twice")
{

    std::shared_ptr<Node> a = Node_Input(2);
    std::shared_ptr<Node> b = Node_Input(-3);
    std::shared_ptr<Node> c = Node_Input(5);
    std::shared_ptr<Node> f = Node_Input(-2);

    std::shared_ptr<Node> d = a*b;
    std::shared_ptr<Node> l = (a*b + d)*f  + (a*c + f)*d;    

    backward(l);

    REQUIRE_THAT(a->get_gradient(), Catch::Matchers::WithinAbs(-42.0, 1e-12));
    REQUIRE_THAT(b->get_gradient(), Catch::Matchers::WithinAbs(8.0, 1e-12));
    REQUIRE_THAT(c->get_gradient(), Catch::Matchers::WithinAbs(-12.0, 1e-12));
    REQUIRE_THAT(d->get_gradient(), Catch::Matchers::WithinAbs(6.0, 1e-12));
    REQUIRE_THAT(f->get_gradient(), Catch::Matchers::WithinAbs(-18.0, 1e-12));

}

