#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "node.h"



TEST_CASE("Gradient Checker")
{

    std::shared_ptr<Node> a = Node_Input(2);
    std::shared_ptr<Node> b = Node_Input(-3);
    std::shared_ptr<Node> c = Node_Input(5);
    std::shared_ptr<Node> f = Node_Input(-2);

    std::shared_ptr<Node> d = a*b;
    std::shared_ptr<Node> l = (a*b + d)*f  + (a*c + f)*d;    

    a->set_gradient(Gradient_checker(a, 0.0001, l)); 
    b->set_gradient(Gradient_checker(b, 0.0001, l)); 
    c->set_gradient(Gradient_checker(c, 0.0001, l)); 
    f->set_gradient(Gradient_checker(f, 0.0001, l));

    REQUIRE_THAT(a->get_gradient(), Catch::Matchers::WithinAbs(-42.0, 1e-8));
    REQUIRE_THAT(b->get_gradient(), Catch::Matchers::WithinAbs(8.0, 1e-8));
    REQUIRE_THAT(c->get_gradient(), Catch::Matchers::WithinAbs(-12.0, 1e-8));
    REQUIRE_THAT(f->get_gradient(), Catch::Matchers::WithinAbs(-18.0, 1e-8));

}

