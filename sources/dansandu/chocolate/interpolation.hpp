#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::interpolation
{

class BarycentricCoordinates
{
public:
    BarycentricCoordinates(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c);

    Vector3 operator()(const ConstantVector3View vertex) const;

private:
    Vector3 origin_;
    Vector3 v0_;
    Vector3 v1_;
    float d00_;
    float d01_;
    float d11_;
    float dnm_;
};

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const float x, const float y,
                    const float epsilon);

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                    const float x, const float y, const float epsilon);

}
