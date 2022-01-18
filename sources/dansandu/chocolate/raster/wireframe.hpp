#pragma once

#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::raster::wireframe
{

void drawWireframe(const ConstantVerticesView vertices, const ConstantTrianglesView triangles,
                   const dansandu::canvas::color::Color solidColor, dansandu::canvas::image::Image& image);

}
