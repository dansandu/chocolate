#include "dansandu/chocolate/geometry/cuboid.hpp"

namespace dansandu::chocolate::geometry::cuboid
{

std::pair<Vertices, Triangles> generateCuboid(const float length, const float height, const float width)
{
    const auto l = length / 2.0f;
    const auto h = height / 2.0f;
    const auto w = width / 2.0f;

    auto vertices = Vertices{24, 4};
    auto triangles = Triangles{12, 3};

    auto vertex = 0;
    auto triangle = 0;
    for (auto axis = 0; axis < 3; ++axis)
    {
        for (auto sign = -1; sign < 2; sign += 2)
        {
            const auto flip = sign > 0;
            triangles(triangle, 0) = vertex + (flip - sign * 0 + 4) % 4;
            triangles(triangle, 1) = vertex + (flip - sign * 1 + 4) % 4;
            triangles(triangle, 2) = vertex + (flip - sign * 2 + 4) % 4;
            ++triangle;

            triangles(triangle, 0) = vertex + (flip - sign * 2 + 4) % 4;
            triangles(triangle, 1) = vertex + (flip - sign * 3 + 4) % 4;
            triangles(triangle, 2) = vertex + (flip - sign * 0 + 4) % 4;
            ++triangle;

            vertices(vertex, 0) = l;
            vertices(vertex, 1) = h;
            vertices(vertex, 2) = w;
            vertices(vertex, 3) = 1.0;
            vertices(vertex, axis) *= sign;
            vertices(vertex, (axis + 1) % 3) *= -1.0f;
            ++vertex;

            vertices(vertex, 0) = l;
            vertices(vertex, 1) = h;
            vertices(vertex, 2) = w;
            vertices(vertex, 3) = 1.0;
            vertices(vertex, axis) *= sign;
            ++vertex;

            vertices(vertex, 0) = l;
            vertices(vertex, 1) = h;
            vertices(vertex, 2) = w;
            vertices(vertex, 3) = 1.0;
            vertices(vertex, axis) *= sign;
            vertices(vertex, (axis + 2) % 3) *= -1.0f;
            ++vertex;

            vertices(vertex, 0) = l;
            vertices(vertex, 1) = h;
            vertices(vertex, 2) = w;
            vertices(vertex, 3) = 1.0;
            vertices(vertex, axis) *= sign;
            vertices(vertex, (axis + 1) % 3) *= -1.0f;
            vertices(vertex, (axis + 2) % 3) *= -1.0f;
            ++vertex;
        }
    }

    return {std::move(vertices), std::move(triangles)};
}

}
