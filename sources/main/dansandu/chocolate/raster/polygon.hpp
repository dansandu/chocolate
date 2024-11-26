#pragma once

#include "dansandu/chocolate/common.hpp"

#include <functional>

namespace dansandu::chocolate::raster::polygon
{

using ShaderType = std::function<void(const ConstantVector3View vertex, const double alpha, const double beta,
                                      const double gamma, const double delta)>;

void drawPolygon(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                 const ConstantVector3View d, const ShaderType& shader, const bool wireframe);

}
