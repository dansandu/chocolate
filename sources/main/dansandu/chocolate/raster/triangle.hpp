#pragma once

#include "dansandu/chocolate/common.hpp"

#include <functional>

namespace dansandu::chocolate::raster::triangle
{

using ShaderType =
    std::function<void(const ConstantVector3View vertex, const double alpha, const double beta, const double gamma)>;

void drawTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                  const ShaderType& shader, const bool wireframe);

}
