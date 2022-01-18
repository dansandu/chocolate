#include "dansandu/chocolate/geometry/clipping.hpp"
#include "dansandu/chocolate/common.hpp"

using dansandu::math::matrix::size_type;
using dansandu::math::matrix::Slicer;

namespace dansandu::chocolate::geometry::clipping
{

std::pair<Vertices, Triangles> clip(const ConstantVerticesView vertices, const ConstantTrianglesView triangles)
{
    auto getVertex = [&](auto triangle, auto vertex)
    { return Slicer<dynamic, 0, 1, 4>::slice(vertices, triangles(triangle, vertex)); };

    auto vertexBuffer = std::vector<float>{};
    auto triangleBuffer = std::vector<size_type>{};

    for (auto triangle = 0; triangle < triangles.rowCount(); ++triangle)
    {
        auto polygon = std::vector<Vector4>{{static_cast<Vector4>(getVertex(triangle, 0)),
                                             static_cast<Vector4>(getVertex(triangle, 1)),
                                             static_cast<Vector4>(getVertex(triangle, 2))}};

        auto clippedPolygon = std::vector<Vector4>{};

        for (auto side = -1.0f; side < 2.0f; side += 2.0f)
        {
            for (auto axis = 0; axis < 3; ++axis)
            {
                for (auto a = 0, b = 1; a < static_cast<size_type>(polygon.size()); b = (++a + 1) % polygon.size())
                {
                    constexpr auto w = 3;

                    if (polygon[a](w) > side * polygon[a](axis))
                    {
                        clippedPolygon.push_back(polygon[a]);
                    }

                    auto t = (side * polygon[a](w) - polygon[a](axis)) /
                             (polygon[b](axis) - polygon[a](axis) - side * (polygon[b](w) - polygon[a](w)));

                    if ((0.0f < t) & (t < 1.0f))
                    {
                        clippedPolygon.push_back(polygon[a] + (polygon[b] - polygon[a]) * t);
                    }
                }

                polygon = std::move(clippedPolygon);
            }
        }

        auto offset = static_cast<size_type>(vertexBuffer.size()) / 4;

        for (auto i = 0; i < static_cast<size_type>(polygon.size()); ++i)
        {
            vertexBuffer.insert(vertexBuffer.end(), polygon[i].begin(), polygon[i].end());

            if (i > 1)
            {
                triangleBuffer.insert(triangleBuffer.end(), {offset, offset + i - 1, offset + i});
            }
        }
    }

    auto vertexCount = static_cast<size_type>(vertexBuffer.size()) / vertices.columnCount();
    auto triangleCount = static_cast<size_type>(triangleBuffer.size()) / triangles.columnCount();

    return {Vertices{vertexCount, 4, std::move(vertexBuffer)}, Triangles{triangleCount, 3, std::move(triangleBuffer)}};
}

Triangles cull(const ConstantVerticesView vertices, const ConstantTrianglesView triangles)
{
    auto getVertex = [&](auto triangle, auto vertex)
    { return Slicer<dynamic, 0, 1, 3>::slice(vertices, triangles(triangle, vertex)); };

    auto triangleBuffer = std::vector<size_type>{};

    for (auto triangle = 0; triangle < triangles.rowCount(); ++triangle)
    {
        auto o = getVertex(triangle, 2);
        auto u = getVertex(triangle, 0) - o;
        auto v = getVertex(triangle, 1) - o;

        if (u.y() * v.x() - u.x() * v.y() < 0.0f)
        {
            triangleBuffer.insert(triangleBuffer.end(),
                                  {triangles(triangle, 0), triangles(triangle, 1), triangles(triangle, 2)});
        }
    }

    auto triangleCount = static_cast<size_type>(triangleBuffer.size()) / triangles.columnCount();

    return {triangleCount, triangles.columnCount(), std::move(triangleBuffer)};
}

}
