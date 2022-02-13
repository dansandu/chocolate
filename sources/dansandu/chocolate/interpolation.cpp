#include "dansandu/chocolate/interpolation.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::math::matrix::close;
using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::dotProduct;

namespace dansandu::chocolate::raster::triangle
{

Vector3 interpolate(const Vector3& a, const Vector3& b, float x, float y)
{
    auto t = static_cast<float>(a.z() > b.z());

    const auto epsilon = 1.0e-5f;
    if (!close(a.x(), b.x(), epsilon))
    {
        t = (x - a.x()) / (b.x() - a.x());
    }        
    else if (!close(a.y(), b.y(), epsilon))
    {
        t = (y - a.y()) / (b.y() - a.y());
    }

    return a + t * (b - a);
}

Vector3 interpolate(const Vector3& a, const Vector3& b, const Vector3& c, float x, float y)
{
    const auto n = crossProduct(a - c, b - c);

    if (close(n.z(), 0.0f))
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
