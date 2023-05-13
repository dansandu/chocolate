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

        const auto shader = [&](const auto vertex, const auto, const auto, const auto)
        { image(vertex.x(), vertex.y()) = color; };

        drawTriangle(a, b, c, shader, false);
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

        const auto shader = [&](const auto vertex, const auto, const auto, const auto)
        { image(vertex.x(), vertex.y()) = color; };

        drawTriangle(a, b, c, shader, true);
    }
}

void drawTexture(const ConstantVerticesView vertices, const ConstantTrianglesView triangles,
                 const ConstantTextureMappingView textureMapping, const Image& texture, Image& image)
{
    const auto getVertex = [&](const int t, const int v) { return Vector3Slicer::slice(vertices, triangles(t, v)); };

    const auto getTextureMapping = [&](const int t, const int v) { return sliceRow(textureMapping, triangles(t, v)); };

    auto tape = std::vector<float>{};

    for (auto t = 0; t < triangles.rowCount(); ++t)
    {
        const auto a = getVertex(t, 0);
        const auto b = getVertex(t, 1);
        const auto c = getVertex(t, 2);

        const auto ta = getTextureMapping(t, 0);
        const auto tb = getTextureMapping(t, 1);
        const auto tc = getTextureMapping(t, 2);

        const auto shader = [&](const auto vertex, const auto u, const auto v, const auto w)
        {
            const auto up = u / a.z();
            const auto vp = v / b.z();
            const auto wp = w / c.z();
            const auto td = getRounded((up * ta + vp * tb + wp * tc) / (up + vp + wp));
            const auto tx = std::min(texture.width() - 1, std::max(0, td.x()));
            const auto ty = std::min(texture.height() - 1, std::max(0, td.y()));
            image(vertex.x(), vertex.y()) = texture(tx, ty);

            tape.push_back(u);
            tape.push_back(v);
            tape.push_back(w);
        };

        drawTriangle(a, b, c, shader, false);
    }
}

}
