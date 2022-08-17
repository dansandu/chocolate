#include "dansandu/chocolate/interpolation.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::math::common::close;
using dansandu::math::matrix::close;
using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::dotProduct;

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
    const auto u = 1.0f - v - w;

    return Vector3{{u, v, w}};
}

Vector3 interpolate(const ConstantVector3View a, const ConstantVector3View b, const float x, const float y,
                    const float epsilon)
{
    auto t = static_cast<float>(a.z() > b.z());

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
                    const float x, const float y, const float epsilon)
{
    const auto n = crossProduct(a - c, b - c);

    if (close(n.z(), 0.0f, epsilon))
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
