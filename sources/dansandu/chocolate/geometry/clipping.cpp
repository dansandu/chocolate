#include "dansandu/chocolate/geometry/clipping.hpp"
#include "dansandu/chocolate/common.hpp"

#include <vector>

using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::dotProduct;
using dansandu::math::matrix::normalized;
using dansandu::math::matrix::size_type;
using dansandu::math::matrix::sliceRow;

namespace dansandu::chocolate::geometry::clipping
{

std::tuple<Vertices, Triangles, Normals> clip(const ConstantVerticesView vertices,
                                              const ConstantTrianglesView triangles, const ConstantNormalsView normals)
{
    const auto getVertex = [&](auto t, auto v) { return sliceRow(vertices, triangles(t, v)); };

    auto vertexBuffer = std::vector<float>{};
    auto triangleBuffer = std::vector<size_type>{};
    auto normalBuffer = std::vector<float>{};

    for (auto t = 0; t < triangles.rowCount(); ++t)
    {
        auto polygon =
            std::vector<Vector4>{{static_cast<Vector4>(getVertex(t, 0)), static_cast<Vector4>(getVertex(t, 1)),
                                  static_cast<Vector4>(getVertex(t, 2))}};

        auto clippedPolygon = std::vector<Vector4>{};

        for (auto side = -1.0f; side < 2.0f; side += 2.0f)
        {
            for (auto axis = 0; axis < 3; ++axis)
            {
                for (auto a = 0, b = 1; a < static_cast<size_type>(polygon.size()); b = (++a + 1) % polygon.size())
                {
                    if (polygon[a].w() > side * polygon[a](axis))
                    {
                        clippedPolygon.push_back(polygon[a]);
                    }

                    auto t = (side * polygon[a].w() - polygon[a](axis)) /
                             (polygon[b](axis) - polygon[a](axis) - side * (polygon[b].w() - polygon[a].w()));

                    if ((0.0f < t) & (t < 1.0f))
                    {
                        clippedPolygon.push_back(polygon[a] + (polygon[b] - polygon[a]) * t);
                    }
                }
                polygon = std::move(clippedPolygon);
            }
        }

        const auto offset = static_cast<size_type>(vertexBuffer.size()) / Vertices::staticColumnCount;

        const auto normal = normals.rowCount() > 0 ? static_cast<Vector3>(sliceRow(normals, t)) : Vector3{};

        for (auto i = 0; i < static_cast<size_type>(polygon.size()); ++i)
        {
            vertexBuffer.insert(vertexBuffer.end(), polygon[i].cbegin(), polygon[i].cend());

            if (i > 1)
            {
                triangleBuffer.insert(triangleBuffer.end(), {offset, offset + i - 1, offset + i});
                normalBuffer.insert(normalBuffer.end(), normal.cbegin(), normal.cend());
            }
        }
    }

    auto vertexCount = static_cast<size_type>(vertexBuffer.size()) / vertices.columnCount();
    auto triangleCount = static_cast<size_type>(triangleBuffer.size()) / triangles.columnCount();

    return {Vertices{vertexCount, vertices.columnCount(), std::move(vertexBuffer)},
            Triangles{triangleCount, triangles.columnCount(), std::move(triangleBuffer)},
            Normals{triangleCount, normals.columnCount(), std::move(normalBuffer)}};
}

std::pair<Triangles, Normals> cull(const ConstantVerticesView vertices, const ConstantTrianglesView triangles)
{
    const auto getVertex = [&](const int t, const int v) { return Vector3Slicer::slice(vertices, triangles(t, v)); };

    auto triangleBuffer = std::vector<size_type>{};
    auto normalBuffer = std::vector<float>{};

    for (auto t = 0; t < triangles.rowCount(); ++t)
    {
        const auto a = getVertex(t, 0);
        const auto b = getVertex(t, 1) - a;
        const auto c = getVertex(t, 2) - a;

        const auto normal = normalized(crossProduct(b - a, c - a));

        if (dotProduct(a, normal) < 0.0f)
        {
            const auto triangle = sliceRow(triangles, t);
            triangleBuffer.insert(triangleBuffer.end(), triangle.cbegin(), triangle.cend());
            normalBuffer.insert(normalBuffer.end(), normal.cbegin(), normal.cend());
        }
    }

    const auto triangleCount = static_cast<size_type>(triangleBuffer.size()) / triangles.columnCount();

    return {Triangles{triangleCount, triangles.columnCount(), std::move(triangleBuffer)},
            Normals{triangleCount, Normals::staticColumnCount, std::move(normalBuffer)}};
}

}
