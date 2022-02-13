#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::interpolation
{

Vector3 interpolate(const Vector3& a, const Vector3& b, const float x, const float y, const float epsilon);

Vector3 interpolate(const Vector3& a, const Vector3& b, const Vector3& c, const float x, const float y,
                    const float epsilon);

}
