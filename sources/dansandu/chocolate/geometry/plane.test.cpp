#include "dansandu/chocolate/geometry/plane.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::chocolate::Triangles;
using dansandu::chocolate::Vertices;
using dansandu::chocolate::geometry::plane::generatePlane;
using dansandu::math::matrix::close;

TEST_CASE("plane")
{
    const auto width = 100.0f;
    const auto height = 50.0f;
    const auto widthResolution = 3;
    const auto heightResolution = 2;
    const auto [vertices, triangles] = generatePlane(width, height, widthResolution, heightResolution);

    const auto expectedVertices = Vertices{{{-50.0f, -25.0f, 0.0f, 1.0f},
                                            {0.0f, -25.0f, 0.0f, 1.0f},
                                            {50.0f, -25.0f, 0.0f, 1.0f},
                                            {-50.0f, 25.0f, 0.0f, 1.0f},
                                            {0.0f, 25.0f, 0.0f, 1.0f},
                                            {50.0f, 25.0f, 0.0f, 1.0f}}};

    const auto expectedTriangles = Triangles{{{0, 1, 4}, {0, 4, 3}, {1, 2, 5}, {1, 5, 4}}};

    REQUIRE(close(vertices, expectedVertices, 1.0e-5f));

    REQUIRE(triangles == expectedTriangles);
}
