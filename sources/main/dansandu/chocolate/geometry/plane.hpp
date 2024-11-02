#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::geometry::plane
{

std::pair<Vertices, Triangles> generatePlane(const double width, const double height, const int widthResolution,
                                             const int heightResolution);

}
