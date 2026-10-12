// Example test: shows the Catch2 syntax. Delete this file once real tests
// exist, and remove it from the list in CMakeLists.txt.

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// A test is a TEST_CASE with a name. The name must be unique.
// REQUIRE stops the test case at the first failure; CHECK records the failure
// and keeps going.
TEST_CASE("example: integers compare exactly") {
    int sum = 2 + 3;
    REQUIRE(sum == 5);
    CHECK(sum != 6);
}

// Doubles need a tolerance: 0.1 + 0.2 == 0.3 is false in floating point.
TEST_CASE("example: doubles compare within a tolerance") {
    double x = 0.1 + 0.2;
    REQUIRE_THAT(x, Catch::Matchers::WithinAbs(0.3, 1e-12));
}
