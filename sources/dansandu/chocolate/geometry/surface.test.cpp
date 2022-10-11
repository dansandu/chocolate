#include "dansandu/chocolate/geometry/surface.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/common.hpp"

using dansandu::chocolate::Normals;
using dansandu::chocolate::Triangles;
using dansandu::chocolate::Vertices;
using dansandu::chocolate::geometry::surface::generateTriangleNormals;
using dansandu::math::matrix::close;

TEST_CASE("surface")
{
    SECTION("normals")
    {
        // clang-format off
        const auto vertices = Vertices{{
            {0.0, 0.0, 0.0, 1.0},
            {1.0, 0.0, 0.0, 1.0},
            {1.0, 1.0, 0.0, 1.0},
            {1.0, 0.0, 1.0, 1.0},
        }};

        const auto triangles = Triangles{{
            {0, 1, 2},
            {3, 1, 2},
        }};
        
        const auto expectedNormals = Normals{{
            {0.0, 0.0, 1.0},
            {1.0, 0.0, 0.0},
        }};
        // clang-format on

        const auto actualNormals = generateTriangleNormals(vertices, triangles);

        REQUIRE(close(actualNormals, expectedNormals, 1.0e-5f));
    }
}
