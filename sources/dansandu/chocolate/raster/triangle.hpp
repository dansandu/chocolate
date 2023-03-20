#pragma once

#include "dansandu/chocolate/common.hpp"

#include <functional>

namespace dansandu::chocolate::raster::triangle
{

using ShaderType = std::function<void(const ConstantVector3View, const float, const float, const float)>;

void drawTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                  const bool fill, const ShaderType& shader);

}
