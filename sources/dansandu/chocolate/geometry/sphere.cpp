#include "dansandu/chocolate/geometry/sphere.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/common.hpp"

using dansandu::math::common::pi;

namespace dansandu::chocolate::geometry::sphere
{

std::pair<Vertices, Triangles> generateSphere(const float radius, const int yResolution, const int zResolution)
{
    if (yResolution < 3)
    {
        THROW(std::invalid_argument, "y resolution must be greater or equal to three -- ", yResolution, " provided");
    }

    if (zResolution < 1)
    {
        THROW(std::invalid_argument, "z resolution must be greater or equal to one -- ", zResolution, " provided");
    }

    const auto vertexCount = yResolution * zResolution + 2;
    const auto triangleCount = 2 * yResolution * zResolution;

    auto vertices = Vertices{vertexCount, 4};
    auto triangles = Triangles{triangleCount, 3};

    auto triangle = 0;
    for (auto j = 0; j < yResolution; ++j)
    {
        triangles(triangle, 0) = 0;
        triangles(triangle, 1) = j + 1;
        triangles(triangle, 2) = (j + 1) % yResolution + 1;
        ++triangle;

        const auto yRadians = 2.0f * j * pi<float> / yResolution;

        for (auto k = 0; k < zResolution; ++k)
        {
            const auto vertex = k * yResolution + j + 1;
            const auto zRadians = (k + 1) * pi<float> / (zResolution + 1) + 0.5 * pi<float>;

            vertices(vertex, 0) = radius * std::cos(zRadians) * std::cos(yRadians);
            vertices(vertex, 1) = radius * std::sin(zRadians);
            vertices(vertex, 2) = -radius * std::cos(zRadians) * std::sin(yRadians);
            vertices(vertex, 3) = 1.0f;

            if (k + 1 < zResolution)
            {
                triangles(triangle, 0) = k * yResolution + (j + 1) % yResolution + 1;
                triangles(triangle, 1) = k * yResolution + j + 1;
                triangles(triangle, 2) = (k + 1) * yResolution + j + 1;
                ++triangle;

                triangles(triangle, 0) = k * yResolution + (j + 1) % yResolution + 1;
                triangles(triangle, 1) = (k + 1) * yResolution + j + 1;
                triangles(triangle, 2) = (k + 1) * yResolution + (j + 1) % yResolution + 1;
                ++triangle;
            }
        }

        triangles(triangle, 0) = (zResolution - 1) * yResolution + (j + 1) % yResolution + 1;
        triangles(triangle, 1) = (zResolution - 1) * yResolution + j + 1;
        triangles(triangle, 2) = zResolution * yResolution + 1;
        ++triangle;
    }

    vertices(0, 0) = 0.0f;
    vertices(0, 1) = radius;
    vertices(0, 2) = 0.0f;
    vertices(0, 3) = 1.0f;

    vertices(vertexCount - 1, 0) = 0.0f;
    vertices(vertexCount - 1, 1) = -radius;
    vertices(vertexCount - 1, 2) = 0.0f;
    vertices(vertexCount - 1, 3) = 1.0f;

    return {std::move(vertices), std::move(triangles)};
}

}
