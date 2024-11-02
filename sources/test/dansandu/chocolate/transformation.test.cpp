#include "dansandu/chocolate/transformation.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::chocolate::Matrix4;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::Vertices;
using dansandu::math::close;
using dansandu::math::pi;
using dansandu::radiance::Tolerance;

using namespace dansandu::chocolate::transformation;

TEST_CASE("transform")
{
    SECTION("shearX")
    {
        const auto actual = shearX(3.0, 7.0);

        // clang-format off
        const auto expected = Matrix4{{{1.0, 3.0, 7.0, 0.0},
                                       {0.0, 1.0, 0.0, 0.0},
                                       {0.0, 0.0, 1.0, 0.0},
                                       {0.0, 0.0, 0.0, 1.0}}};
        // clang-format on

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("scale")
    {
        const auto actual = scale(2.0, 3.0, 4.0);

        // clang-format off
        const auto expected = Matrix4{{{2.0, 0.0, 0.0, 0.0},
                                       {0.0, 3.0, 0.0, 0.0},
                                       {0.0, 0.0, 4.0, 0.0},
                                       {0.0, 0.0, 0.0, 1.0}}};
        // clang-format on

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("translate")
    {
        const auto actual = translate(7.0, 11.0, 13.0);

        // clang-format off
        const auto expected = Matrix4{{{1.0, 0.0, 0.0,  7.0},
                                       {0.0, 1.0, 0.0, 11.0},
                                       {0.0, 0.0, 1.0, 13.0}, 
                                       {0.0, 0.0, 0.0,  1.0}}};
        // clang-format on

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("rotate by X axis")
    {
        const auto actual = rotateByX(pi<double> / 2.0);

        // clang-format off
        const auto expected = Matrix4{{{1.0, 0.0,  0.0, 0.0},
                                       {0.0, 0.0, -1.0, 0.0},
                                       {0.0, 1.0,  0.0, 0.0},
                                       {0.0, 0.0,  0.0, 1.0}}};
        // clang-format on

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("rotate by Y axis")
    {
        const auto actual = rotateByY(pi<double> / 2.0);

        // clang-format off
        const auto expected = Matrix4{{{ 0.0, 0.0, 1.0, 0.0}, 
                                       { 0.0, 1.0, 0.0, 0.0},
                                       {-1.0, 0.0, 0.0, 0.0},
                                       { 0.0, 0.0, 0.0, 1.0}}};
        // clang-format on

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("rotate by Z axis")
    {
        const auto actual = rotateByZ(pi<double> / 2.0);

        // clang-format off
        const auto expected = Matrix4{{{0.0, -1.0, 0.0, 0.0},
                                       {1.0,  0.0, 0.0, 0.0},
                                       {0.0,  0.0, 1.0, 0.0},
                                       {0.0,  0.0, 0.0, 1.0}}};
        // clang-format on 

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("look at")
    {
        const auto eye = Vector3{{2.0, 0.0, -5.0}};
        const auto target = Vector3{{-50.0, 10.0, 2000.0}};
        const auto up = Vector3{{0.0, 1.0, 0.0}};
        
        const auto actual = lookAt(eye, target, up);

        // clang-format off
        const auto expected = Matrix4{{{-0.999664000,  0.00000000, -0.02592640,  1.8697000},
                                       { 0.000129264,  0.99998800, -0.00498412, -0.0251791},
                                       { 0.025926100, -0.00498579, -0.99965100, -5.0501100},
                                       { 0.000000000,  0.00000000,  0.00000000,  1.0000000}}};
        // clang-format on

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("perspective")
    {
        const auto near = 1.0;
        const auto far = 2000.0;
        const auto fieldOfView = 1.92;
        const auto aspect = 1.5;

        const auto actual = perspective(near, far, fieldOfView, aspect);

        // clang-format off
        const auto expected = Matrix4{{{0.7, 0.00,  0.0,  0.0},
                                       {0.0, 1.05,  0.0,  0.0},
                                       {0.0, 0.00, -1.0, -2.0},
                                       {0.0, 0.00, -1.0,  0.0}}};
        // clang-format on

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("viewport")
    {
        const auto width = 600.0;
        const auto height = 400.0;

        const auto actual = viewport(width, height);

        // clang-format off
        const auto expected = Matrix4{{{299.5,    0.0, 0.0, 299.5},
                                       {  0.0, -199.5, 0.0, 199.5},
                                       {  0.0,    0.0, 1.0,  -1.0},
                                       {  0.0,    0.0, 0.0,   1.0}}};
        // clang-format on

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("dehomogenized")
    {
        // clang-format off
        const auto vertices = Vertices{{
            {10.0, 50.0, 0.0, 2.0}, 
            {15.0, 5.0, 20.0, 5.0}
        }};

        const auto expected = Vertices{{
            {5.0, 25.0, 0.0, 1.0}, 
            {3.0,  1.0, 4.0, 1.0}
        }};
        // clang-format on

        const auto actual = dehomogenized(vertices);

        REQUIRE(actual == Tolerance(expected));
    }
}
