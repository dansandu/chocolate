#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::geometry::clipping
{

std::pair<Vertices, Triangles> clip(const ConstantVerticesView vertices, const ConstantTrianglesView triangles);

Triangles cull(const ConstantVerticesView vertices, const ConstantTrianglesView triangles);

}
