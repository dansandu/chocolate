#pragma once

#include "dansandu/chocolate/common.hpp"

#include <functional>

namespace dansandu::chocolate::raster::triangle
{

void drawWireframeTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                           const std::function<void(ConstantVector3View, float, float, float)>& shader);

void drawSolidTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                       const std::function<void(ConstantVector3View, float, float, float)>& shader);

}
