#include "dansandu/chocolate/geometry/surface.hpp"
#include "dansandu/chocolate/common.hpp"

using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::normalized;
using dansandu::math::matrix::Slicer;
using dansandu::math::matrix::sliceRow;
using dansandu::math::matrix::transposed;

namespace dansandu::chocolate::geometry::surface
{

Normals generateTriangleNormals(const ConstantVerticesView vertices, const ConstantTrianglesView triangles)
{
    auto normals = Normals{triangles.rowCount(), Normals::staticColumnCount};

    const auto getVertex = [&](const int t, const int v) { return Vector3Slicer::slice(vertices, triangles(t, v)); };

    for (auto t = 0; t < triangles.rowCount(); ++t)
    {
        const auto a = getVertex(t, 2);
        const auto b = getVertex(t, 0);
        const auto c = getVertex(t, 1);

        sliceRow(normals, t).deepCopy(transposed(normalized(crossProduct(b - a, c - a))));
    }

    return normals;
}

}
