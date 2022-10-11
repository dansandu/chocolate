#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::geometry::surface
{

Normals generateTriangleNormals(const ConstantVerticesView vertices, const ConstantTrianglesView triangles);

}
