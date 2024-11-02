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
    double d00_;
    double d01_;
    double d11_;
    double dnm_;
};

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const double x, const double y,
                    const double epsilon);

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                    const double x, const double y, const double epsilon);

}
