#include "dansandu/chocolate/raster/wireframe.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/raster/triangle.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::canvas::color::Color;
using dansandu::canvas::image::Image;
using dansandu::chocolate::raster::triangle::drawWireframeTriangle;
using dansandu::math::matrix::Slicer;

namespace dansandu::chocolate::raster::wireframe
{

void drawWireframe(const ConstantVerticesView vertices, const ConstantTrianglesView triangles, const Color solidColor,
                   Image& image)
{
    auto getPoint = [&](const int triangle, const int vertex)
    {
        const auto view = Slicer<dynamic, 0, 1, 2>::slice(vertices, triangles(triangle, vertex));
        return Point2{{static_cast<int>(std::round(view.x())), static_cast<int>(std::round(view.y()))}};
    };

    for (auto triangle = 0; triangle < triangles.rowCount(); ++triangle)
    {
        auto a = getPoint(triangle, 0);
        auto b = getPoint(triangle, 1);
        auto c = getPoint(triangle, 2);

        drawWireframeTriangle(a, b, c, [&image, solidColor](auto point) { image(point) = solidColor; });
    }
}

}
