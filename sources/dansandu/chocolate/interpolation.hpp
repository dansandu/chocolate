#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::interpolation
{

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const float x, const float y,
                    const float epsilon);

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                    const float x, const float y, const float epsilon);

}
