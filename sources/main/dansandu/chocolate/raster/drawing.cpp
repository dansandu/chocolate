#include "dansandu/chocolate/raster/drawing.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/chocolate/raster/polygon.hpp"
#include "dansandu/chocolate/raster/triangle.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::canvas::color::Color;
using dansandu::canvas::image::Image;
using dansandu::chocolate::raster::polygon::drawPolygon;
using dansandu::chocolate::raster::triangle::drawTriangle;
using dansandu::math::matrix::dotProduct;
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
        const auto gamma = std::max(0.0, dotProduct(-diffuseDirection, normal));
        const auto color = toColor(objectColorVec + diffuseColorVec * gamma);

        const auto shader = [&](const auto vertex, const auto, const auto, const auto)
        { image(vertex.x(), vertex.y()) = color; };

        const auto wireframe = false;

        drawTriangle(a, b, c, wireframe, shader);
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

        const auto wireframe = true;

        drawTriangle(a, b, c, wireframe, shader);
    }
}

void drawTexture(const ConstantVerticesView vertices, const ConstantTrianglesView triangles,
                 const ConstantTextureMappingView textureMapping, const Image& texture, Image& image)
{
    const auto getVertex = [&](const int t, const int v) { return Vector3Slicer::slice(vertices, triangles(t, v)); };

    const auto getTextureMapping = [&](const int t, const int v) { return sliceRow(textureMapping, triangles(t, v)); };

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
        };

        const auto wireframe = false;

        drawTriangle(a, b, c, wireframe, shader);
    }
}

void drawTexture(const ConstantVerticesView vertices, const ConstantPolygonsView polygons,
                 const ConstantTextureMappingView textureMapping, const dansandu::canvas::image::Image& texture,
                 dansandu::canvas::image::Image& image)
{
    const auto getVertex = [&](const int i, const int v) { return Vector3Slicer::slice(vertices, polygons(i, v)); };

    const auto getTextureMapping = [&](const int i, const int v) { return sliceRow(textureMapping, polygons(i, v)); };

    for (auto i = 0; i < polygons.rowCount(); ++i)
    {
        const auto a = getVertex(i, 0);
        const auto b = getVertex(i, 1);
        const auto c = getVertex(i, 2);
        const auto d = getVertex(i, 3);

        const auto ta = getTextureMapping(i, 0);
        const auto tb = getTextureMapping(i, 1);
        const auto tc = getTextureMapping(i, 2);
        const auto td = getTextureMapping(i, 3);

        const auto shader =
            [&](const auto vertex, const auto alpha, const auto beta, const auto gamma, const auto delta)
        {
            const auto tv = getRounded(alpha * ta + beta * tb + gamma * tc + delta * td);
            const auto tx = std::min(texture.width() - 1, std::max(0, tv.x()));
            const auto ty = std::min(texture.height() - 1, std::max(0, tv.y()));

            image(vertex.x(), vertex.y()) = texture(tx, ty);
        };

        const auto wireframe = false;

        drawPolygon(a, b, c, d, wireframe, shader);
    }
}

}
