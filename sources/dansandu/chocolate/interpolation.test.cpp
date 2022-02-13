#include "dansandu/chocolate/interpolation.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/chocolate/common.hpp"

using dansandu::chocolate::Vector3;
using dansandu::chocolate::interpolation::interpolate;
using dansandu::math::matrix::close;

TEST_CASE("interpolation")
{
    const auto epsilon = 1.0e-5f;

    const auto a = Vector3{{2.0f, -10.0f, 20.0f}};
    const auto b = Vector3{{-50.0f, 80.0f, -100.0f}};

    SECTION("line")
    {
        const auto t = 0.25f;

        const auto expected = a + (b - a) * t;
        const auto actual = interpolate(a, b, expected.x(), expected.y(), epsilon);

        REQUIRE(close(actual, expected, epsilon));
    }

    SECTION("plane")
    {
        const auto c = Vector3{{33.0f, 10.0f, -77.0f}};

        const auto u = 0.20f;
        const auto v = 0.70f;

        const auto expected = c + (a - c) * u + (b - c) * v;
        const auto actual = interpolate(a, b, c, expected.x(), expected.y(), epsilon);

        REQUIRE(close(actual, expected, epsilon));
    }
}
