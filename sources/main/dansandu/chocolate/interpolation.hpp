#pragma once

#include "dansandu/chocolate/common.hpp"

#include <optional>

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

class Line
{
public:
    Line(const ConstantVector2View a, const ConstantVector2View b);

    bool isVertical(const double tolerance = 1.0e-12) const;

    double slope() const;

    bool intersect(const Line& other, const Vector2View intersection, const double tolerance = 1.0e-12) const;

    double solveParametric(const ConstantVector2View point, const double tolerance = 1.0e-12) const;

private:
    Vector2 a_;
    Vector2 b_;
};

bool isConvexPolygon(const ConstantVector2View a, const ConstantVector2View b, const ConstantVector2View c,
                     const ConstantVector2View d);

class BilinearInterpolation
{
public:
    BilinearInterpolation(const ConstantVector2View vanishingPoint, const Line& vanishingLine1,
                          const Line& vanishingLine2, const Line& transversal1, const Line& transversal2,
                          const bool shiftPoints);

    Vector4 operator()(const ConstantVector3View vertex) const;

private:
    Vector2 vanishingPoint_;
    Line vanishingLine1_;
    Line vanishingLine2_;
    Line transversal1_;
    Line transversal2_;
    bool shiftPoints_;
};

std::optional<BilinearInterpolation> canInterpolateBilineary(const ConstantVector3View a, const ConstantVector3View b,
                                                             const ConstantVector3View c, const ConstantVector3View d);

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const double x, const double y,
                    const double epsilon);

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                    const double x, const double y, const double epsilon);

}
