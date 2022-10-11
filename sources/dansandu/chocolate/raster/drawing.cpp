#include "dansandu/chocolate/raster/drawing.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/chocolate/raster/triangle.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::canvas::color::Color;
using dansandu::canvas::image::Image;
using dansandu::chocolate::raster::triangle::drawTriangle;
using dansandu::math::matrix::dotProduct;
using dansandu::math::matrix::magnitude;
using dansandu::math::matrix::normalized;
using dansandu::math::matrix::Slicer;

namespace dansandu::chocolate::raster::drawing
{

void drawFlat(const ConstantVerticesView vertices, const ConstantTrianglesView triangles,
              const ConstantNormalsView normals, const Color objectColor, const Color diffuseColor,
              const ConstantVector3View diffuseDirection, Image& image)
{
    const auto getVertex = [&](const int t, const int v) { return Vector3Slicer::slice(vertices, triangles(t, v)); };

    const auto getNormal = [&](const int t) { return Vector3Slicer::slice(normals, t); };

    const auto objectColorVec = toVector3(objectColor);
    const auto diffuseColorVec = toVector3(diffuseColor);

    for (auto t = 0; t < triangles.rowCount(); ++t)
    {
        const auto a = getVertex(t, 0);
        const auto b = getVertex(t, 1);
        const auto c = getVertex(t, 2);

        const auto normal = getNormal(t);
        const auto gamma = std::max(0.0f, dotProduct(-diffuseDirection, normal));
        const auto color = toColor(objectColorVec + diffuseColorVec * gamma);

        drawTriangle(a, b, c, true,
                     [&image, color](auto vertex, auto, auto, auto) { image(vertex.x(), vertex.y()) = color; });
    }
}

void drawWireframe(const ConstantVerticesView vertices, const ConstantTrianglesView triangles, const Color color,
                   Image& image)
{
    auto slice = [&](const int triangle, const int vertex)
    { return Vector3Slicer::slice(vertices, triangles(triangle, vertex)); };

    for (auto triangle = 0; triangle < triangles.rowCount(); ++triangle)
    {
        auto a = slice(triangle, 0);
        auto b = slice(triangle, 1);
        auto c = slice(triangle, 2);

        drawTriangle(a, b, c, false,
                     [&image, color](auto vertex, auto, auto, auto) { image(vertex.x(), vertex.y()) = color; });
    }
}

}
