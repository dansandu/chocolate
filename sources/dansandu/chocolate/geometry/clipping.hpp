#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::geometry::clipping
{

std::tuple<Vertices, Triangles, Normals> clip(const ConstantVerticesView vertices,
                                              const ConstantTrianglesView triangles, const ConstantNormalsView normals);

std::pair<Triangles, Normals> cull(const ConstantVerticesView vertices, const ConstantTrianglesView triangles);

}
