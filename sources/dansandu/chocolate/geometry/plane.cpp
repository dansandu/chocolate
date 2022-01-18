#include "dansandu/chocolate/geometry/plane.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/common.hpp"

namespace dansandu::chocolate::geometry::plane
{

std::pair<Vertices, Triangles> generatePlane(const float width, const float height, const int widthResolution,
                                             const int heightResolution)
{
    if (widthResolution < 2)
    {
        THROW(std::invalid_argument, "width resolution must be greater or equal to two -- ", widthResolution,
              " provided");
    }

    if (heightResolution < 2)
    {
        THROW(std::invalid_argument, "height resolution must be greater or equal to two -- ", heightResolution,
              " provided");
    }

    const auto vertexCount = widthResolution * heightResolution;
    const auto triangleCount = 2 * (widthResolution - 1) * (heightResolution - 1);

    auto result = std::make_pair(Vertices{vertexCount, 4}, Triangles{triangleCount, 3});

    auto& vertices = result.first;
    auto& triangles = result.second;

    const auto xDelta = width / (widthResolution - 1);
    const auto yDelta = height / (heightResolution - 1);

    auto triangle = 0;
    for (auto vertex = 0; vertex < vertexCount; ++vertex)
    {
        const auto i = vertex % widthResolution;
        const auto j = vertex / widthResolution;
        vertices(vertex, 0) = i * xDelta - width / 2.0f;
        vertices(vertex, 1) = j * yDelta - height / 2.0f;
        vertices(vertex, 3) = 1.0f;

        if (i + 1 != widthResolution && j + 1 != heightResolution)
        {
            triangles(triangle, 0) = vertex;
            triangles(triangle, 1) = vertex + 1;
            triangles(triangle, 2) = vertex + widthResolution + 1;
            ++triangle;

            triangles(triangle, 0) = vertex;
            triangles(triangle, 1) = vertex + widthResolution + 1;
            triangles(triangle, 2) = vertex + widthResolution;
            ++triangle;
        }
    }

    return result;
}

}
