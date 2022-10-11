#pragma once

#include "dansandu/chocolate/common.hpp"

#include <functional>

namespace dansandu::chocolate::raster::triangle
{

void drawTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                  const bool fill, const std::function<void(ConstantVector3View, float, float, float)>& shader);

}
