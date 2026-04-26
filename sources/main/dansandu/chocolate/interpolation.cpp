#include "dansandu/chocolate/interpolation.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/journey/exception.hpp"
#include "dansandu/math/matrix.hpp"

#include <cmath>

using dansandu::math::close;
using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::dotProduct;
using dansandu::math::matrix::Slicer;

namespace dansandu::chocolate::interpolation
{

BarycentricCoordinates::BarycentricCoordinates(const ConstantVector3View a, const ConstantVector3View b,
                                               const ConstantVector3View c)
    : origin_{a},
      v0_{b - a},
      v1_{c - a},
      d00_{dotProduct(v0_, v0_)},
      d01_{dotProduct(v0_, v1_)},
      d11_{dotProduct(v1_, v1_)},
      dnm_{d00_ * d11_ - d01_ * d01_}
{
}

Vector3 BarycentricCoordinates::operator()(const ConstantVector3View vertex) const
{
    const auto v2 = vertex - origin_;
    const auto d20 = dotProduct(v2, v0_);
    const auto d21 = dotProduct(v2, v1_);

    const auto v = (d11_ * d20 - d01_ * d21) / dnm_;
    const auto w = (d00_ * d21 - d01_ * d20) / dnm_;
    const auto u = 1.0 - v - w;

    return Vector3{{u, v, w}};
}

Line::Line(const ConstantVector2View a, const ConstantVector2View b) : a_{a}, b_{b}
{
}

bool Line::isVertical(const double tolerance) const
{
    return std::abs(b_.x() - a_.x()) < tolerance;
}

double Line::slope() const
{
    return (b_.y() - a_.y()) / (b_.x() - a_.x());
}

bool Line::intersect(const Line& other, const Vector2View intersection, const double tolerance) const
{
    const auto vertical1 = isVertical(tolerance);
    const auto vertical2 = other.isVertical(tolerance);

    if (!vertical1 && !vertical2)
    {
        const auto m1 = slope();
        const auto m2 = other.slope();

        if (std::abs(m1 - m2) >= tolerance)
        {
            intersection.x() = (m1 * a_.x() - m2 * other.a_.x() + other.a_.y() - a_.y()) / (m1 - m2);
            intersection.y() = m1 * (intersection.x() - a_.x()) + a_.y();
            return true;
        }
    }
    else if (!vertical1 && vertical2)
    {
        intersection.x() = other.a_.x();
        intersection.y() = slope() * (intersection.x() - a_.x()) + a_.y();
        return true;
    }
    else if (vertical1 && !vertical2)
    {
        intersection.x() = a_.x();
        intersection.y() = other.slope() * (intersection.x() - other.a_.x()) + other.a_.y();
        return true;
    }

    return false;
}

double Line::solveParametric(const ConstantVector2View point, const double tolerance) const
{
    const auto dx = b_.x() - a_.x();
    const auto dy = b_.y() - a_.y();
    const auto x = point.x() - a_.x();
    const auto y = point.y() - a_.y();

    if (std::abs(dx) > tolerance)
    {
        return x / dx;
    }

    if (std::abs(dy) > tolerance)
    {
        return y / dy;
    }

    return 0.0;
}

bool isConvexPolygon(const ConstantVector2View a, const ConstantVector2View b, const ConstantVector2View c,
                     const ConstantVector2View d)
{
    const auto cross = [](const auto& a, const auto& b) { return a.x() * b.y() - a.y() * b.x(); };

    const auto turn1 = cross(b - a, c - b);
    const auto turn2 = cross(c - b, d - c);
    const auto turn3 = cross(d - c, a - d);
    const auto turn4 = cross(a - d, b - a);

    return ((turn1 > 0.0) & (turn2 > 0.0) & (turn3 > 0.0) & (turn4 > 0.0)) |
           ((turn1 < 0.0) & (turn2 < 0.0) & (turn3 < 0.0) & (turn4 < 0.0));
}

ParallelogramInterpolation::ParallelogramInterpolation(const ConstantVector3View a, const ConstantVector3View b,
                                                       const ConstantVector3View c)
    : a_{a}, b_{b}, ab_{b - a}, bc_{c - b}
{
    dba_ = dotProduct(ab_, ab_);
    dcb_ = dotProduct(bc_, bc_);
}

Vector4 ParallelogramInterpolation::operator()(const ConstantVector3View vertex) const
{
    const auto t1 = dotProduct(vertex - a_, ab_) / dba_;
    const auto t2 = dotProduct(vertex - b_, bc_) / dcb_;

    const auto alpha = (1.0 - t1) * (1.0 - t2);
    const auto beta = t1 * (1.0 - t2);
    const auto gamma = t1 * t2;
    const auto delta = (1.0 - t1) * t2;

    return Vector4{{alpha, beta, gamma, delta}};
}

ConvexQuadrilateralInterpolation::ConvexQuadrilateralInterpolation(const ConstantVector2View vanishingPoint,
                                                                   const Line& vanishingLine1,
                                                                   const Line& vanishingLine2, const Line& transversal1,
                                                                   const Line& transversal2, const bool shiftPoints)
    : vanishingPoint_{vanishingPoint},
      vanishingLine1_{vanishingLine1},
      vanishingLine2_{vanishingLine2},
      transversal1_{transversal1},
      transversal2_{transversal2},
      shiftPoints_{shiftPoints}
{
}

Vector4 ConvexQuadrilateralInterpolation::operator()(const ConstantVector3View vertex) const
{
    const auto slice = [](const auto& vector) { return Slicer<0, 0, 1, 2>::slice(vector); };

    const auto point = slice(vertex);

    const auto vanishingLine = Line{vanishingPoint_, point};

    auto middle1 = Vector2{};

    if (!vanishingLine.intersect(transversal1_, middle1))
    {
        THROW(std::logic_error, "no intersection between vanishing line and first transversal line");
    }

    auto middle2 = Vector2{};

    if (!vanishingLine.intersect(transversal2_, middle2))
    {
        THROW(std::logic_error, "no intersection between vanishing line and first transversal line");
    }

    const auto scanLine = Line{middle1, middle2};

    const auto t1 = transversal1_.solveParametric(middle1);
    const auto t2 = transversal2_.solveParametric(middle2);
    const auto t3 = scanLine.solveParametric(point);

    const auto alpha = t1 * (1.0 - t3);
    const auto beta = (1.0 - t2) * t3;
    const auto gamma = t2 * t3;
    const auto delta = (1.0 - t1) * (1.0 - t3);

    if (shiftPoints_)
    {
        return Vector4{{beta, gamma, delta, alpha}};
    }
    else
    {
        return Vector4{{alpha, beta, gamma, delta}};
    }
}

BilinearInterpolation::BilinearInterpolation(const ConstantVector3View a, const ConstantVector3View b,
                                             const ConstantVector3View c, const ConstantVector3View d)
{
    const auto slice = [](const auto& vector) { return Slicer<0, 0, 1, 2>::slice(vector); };

    const auto sa = slice(a);
    const auto sb = slice(b);
    const auto sc = slice(c);
    const auto sd = slice(d);

    if (isConvexPolygon(sa, sb, sc, sd))
    {
        const auto ab = Line{sa, sb};
        const auto bc = Line{sb, sc};
        const auto cd = Line{sc, sd};
        const auto da = Line{sd, sa};

        auto vanishingPoint = Vector2{};

        if (ab.intersect(cd, vanishingPoint))
        {
            interpolation_ = ConvexQuadrilateralInterpolation{vanishingPoint, ab, cd, da, bc, false};
        }
        else if (da.intersect(bc, vanishingPoint))
        {
            interpolation_ = ConvexQuadrilateralInterpolation{vanishingPoint, da, bc, cd, ab, true};
        }
        else
        {
            interpolation_ = ParallelogramInterpolation{a, b, c};
        }
    }
    else
    {
        THROW(std::logic_error, "cannot interpolate a non-convex polygon");
    }
}

Vector4 BilinearInterpolation::operator()(const ConstantVector3View vertex) const
{
    if (const auto pointer = std::get_if<ConvexQuadrilateralInterpolation>(&interpolation_); pointer != nullptr)
    {
        return (*pointer)(vertex);
    }
    else
    {
        return std::get<ParallelogramInterpolation>(interpolation_)(vertex);
    }
}

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const double x, const double y,
                    const double epsilon)
{
    auto t = static_cast<double>(a.z() > b.z());

    if (!close(a.x(), b.x(), epsilon))
    {
        t = (x - a.x()) / (b.x() - a.x());
    }
    else if (!close(a.y(), b.y(), epsilon))
    {
        t = (y - a.y()) / (b.y() - a.y());
    }

    const auto z = a.z() + (b.z() - a.z()) * t;

    return Vector3{{x, y, z}};
}

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                    const double x, const double y, const double epsilon)
{
    const auto n = crossProduct(a - c, b - c);

    if (close(n.z(), 0.0, epsilon))
    {
        const auto z = std::min(a.z(), std::min(b.z(), c.z()));
        return Vector3{{x, y, z}};
    }
    else
    {
        const auto z = (n.x() * x + n.y() * y - dotProduct(c, n)) / -n.z();
        return Vector3{{x, y, z}};
    }
}

}
