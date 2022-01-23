#pragma once

#include "dansandu/chocolate/common.hpp"

#include <functional>

namespace dansandu::chocolate::raster::triangle
{

void drawWireframeTriangle(const ConstantPoint2View a, const ConstantPoint2View b, const ConstantPoint2View c,
                           const std::function<void(ConstantPoint2View)>& shader);

void drawTriangle(const ConstantPoint2View a, const ConstantPoint2View b, const ConstantPoint2View c,
                  const std::function<void(ConstantPoint2View)>& shader);

}
