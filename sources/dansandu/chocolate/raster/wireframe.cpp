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
    auto slice = [&](const int triangle, const int vertex)
    { return Slicer<dynamic, 0, 1, 3>::slice(vertices, triangles(triangle, vertex)); };

    for (auto triangle = 0; triangle < triangles.rowCount(); ++triangle)
    {
        auto a = slice(triangle, 0);
        auto b = slice(triangle, 1);
        auto c = slice(triangle, 2);

        drawWireframeTriangle(a, b, c,
                              [&image, solidColor](auto point, auto, auto, auto)
                              {
                                  const auto p = Point2{{static_cast<int>(std::round(point.x())),
                                                         static_cast<int>(std::round(point.y()))}};
                                  image(p) = solidColor;
                              });
    }
}

}
