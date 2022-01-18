#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::geometry::sphere
{

std::pair<Vertices, Triangles> generateSphere(const float radius, const int yResolution, const int zResolution);

}
