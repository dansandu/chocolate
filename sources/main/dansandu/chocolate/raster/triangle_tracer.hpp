#pragma once

#include "dansandu/chocolate/common.hpp"

#include <functional>

namespace dansandu::chocolate::raster::triangle_tracer
{

void traceTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                   const bool wireframe, const std::function<void(const ConstantVector3View vertex)>& callback);

}
