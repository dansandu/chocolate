#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::geometry::plane
{

std::pair<Vertices, Triangles> generatePlane(const float width, const float height, const int widthResolution,
                                             const int heightResolution);

}
