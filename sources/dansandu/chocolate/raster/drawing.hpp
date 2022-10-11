#pragma once

#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::raster::drawing
{

void drawFlat(const ConstantVerticesView vertices, const ConstantTrianglesView triangles,
              const ConstantNormalsView normals, const dansandu::canvas::color::Color objectColor,
              const dansandu::canvas::color::Color diffuseColor, const ConstantVector3View diffuseDirection,
              dansandu::canvas::image::Image& image);

void drawWireframe(const ConstantVerticesView vertices, const ConstantTrianglesView triangles,
                   const dansandu::canvas::color::Color color, dansandu::canvas::image::Image& image);

}
