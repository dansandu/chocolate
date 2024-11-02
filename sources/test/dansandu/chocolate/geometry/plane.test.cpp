#include "dansandu/chocolate/geometry/plane.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/matrix.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::chocolate::Triangles;
using dansandu::chocolate::Vertices;
using dansandu::chocolate::geometry::plane::generatePlane;
using dansandu::math::close;
using dansandu::radiance::Tolerance;

TEST_CASE("plane")
{
    const auto width = 100.0;
    const auto height = 50.0;
    const auto widthResolution = 3;
    const auto heightResolution = 2;
    const auto [vertices, triangles] = generatePlane(width, height, widthResolution, heightResolution);

    const auto expectedVertices = Vertices{{{-50.0, -25.0, 0.0, 1.0},
                                            {0.0, -25.0, 0.0, 1.0},
                                            {50.0, -25.0, 0.0, 1.0},
                                            {-50.0, 25.0, 0.0, 1.0},
                                            {0.0, 25.0, 0.0, 1.0},
                                            {50.0, 25.0, 0.0, 1.0}}};

    const auto expectedTriangles = Triangles{{{0, 1, 4}, {0, 4, 3}, {1, 2, 5}, {1, 5, 4}}};

    REQUIRE(vertices == Tolerance(expectedVertices));

    REQUIRE(triangles == expectedTriangles);
}
