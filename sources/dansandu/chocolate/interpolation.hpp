#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::raster::triangle
{

Vector3 interpolate(const Vector3& a, const Vector3& b, float x, float y);

Vector3 interpolate(const Vector3& a, const Vector3& b, const Vector3& c, float x, float y);

}
