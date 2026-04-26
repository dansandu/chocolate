#include "dansandu/chocolate/transformation.hpp"
#include "dansandu/math/common.hpp"

using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::dotProduct;
using dansandu::math::matrix::normalized;

namespace dansandu::chocolate::transformation
{

Matrix4 shearX(const double y, const double z)
{
    // clang-format off
    return Matrix4{{{1.0,   y,   z, 0.0},
                    {0.0, 1.0, 0.0, 0.0},
                    {0.0, 0.0, 1.0, 0.0},
                    {0.0, 0.0, 0.0, 1.0}}};
    // clang-format on
}

Matrix4 scale(const double x, const double y, const double z)
{
    // clang-format off
    return Matrix4{{{  x, 0.0, 0.0, 0.0},
                    {0.0,   y, 0.0, 0.0},
                    {0.0, 0.0,   z, 0.0},
                    {0.0, 0.0, 0.0, 1.0}}};
    // clang-format on
}

Matrix4 translate(const double x, const double y, const double z)
{
    // clang-format off
    return Matrix4{{{1.0, 0.0, 0.0,   x},
                    {0.0, 1.0, 0.0,   y},
                    {0.0, 0.0, 1.0,   z},
                    {0.0, 0.0, 0.0, 1.0}}};
    // clang-format on
}

Matrix4 translate(const ConstantVector3View vector)
{
    return translate(vector.x(), vector.y(), vector.z());
}

Matrix4 rotateByX(const double radians)
{
    const auto cos = std::cos(radians);
    const auto sin = std::sin(radians);
    // clang-format off
    return Matrix4{{{1.0, 0.0,  0.0, 0.0},
                    {0.0, cos, -sin, 0.0},
                    {0.0, sin,  cos, 0.0},
                    {0.0, 0.0,  0.0, 1.0}}};
    // clang-format on
}

Matrix4 rotateByY(const double radians)
{
    const auto cos = std::cos(radians);
    const auto sin = std::sin(radians);
    // clang-format off
    return Matrix4{{{ cos, 0.0, sin, 0.0},
                    { 0.0, 1.0, 0.0, 0.0},
                    {-sin, 0.0, cos, 0.0},
                    { 0.0, 0.0, 0.0, 1.0}}};
    // clang-format on
}

Matrix4 rotateByZ(const double radians)
{
    const auto cos = std::cos(radians);
    const auto sin = std::sin(radians);
    // clang-format off
    return Matrix4{{{cos, -sin, 0.0, 0.0},
                    {sin,  cos, 0.0, 0.0},
                    {0.0,  0.0, 1.0, 0.0},
                    {0.0,  0.0, 0.0, 1.0}}};
    // clang-format on
}

Matrix4 lookAt(const ConstantVector3View eye, const ConstantVector3View target, const ConstantVector3View up)
{
    const auto w = normalized(eye - target);
    const auto u = normalized(crossProduct(up, w));
    const auto v = crossProduct(w, u);
    // clang-format off
    const auto base = Matrix4{{{u.x(), u.y(), u.z(), 0.0},
                               {v.x(), v.y(), v.z(), 0.0},
                               {w.x(), w.y(), w.z(), 0.0},
                               {  0.0,   0.0,   0.0, 1.0}}};
    // clang-format on
    return base * translate(-eye);
}

Matrix4 perspective(const double near, const double far, const double fieldOfViewRadians, const double aspect)
{
    const auto ctg = 1.0 / std::tan(0.5 * fieldOfViewRadians);
    const auto p33 = (far + near) / (near - far);
    const auto p34 = 2.0 * far * near / (near - far);
    // clang-format off
    return Matrix4{{{ctg,          0.0,  0.0, 0.0}, 
                    {0.0, aspect * ctg,  0.0, 0.0},
                    {0.0,          0.0,  p33, p34},
                    {0.0,          0.0, -1.0, 0.0}}};
    // clang-format on
}

Matrix4 viewport(const double width, const double height)
{
    const auto x = (width - 1.0) / 2.0;
    const auto y = (height - 1.0) / 2.0;
    return translate(x, y, -1.0) * scale(x, -y, 1.0);
}

Vertices dehomogenized(const ConstantVerticesView vertices)
{
    auto result = static_cast<Vertices>(vertices);
    for (auto i = 0; i < result.rowCount(); ++i)
    {
        for (auto j = 0; j + 1 < result.columnCount(); ++j)
        {
            result(i, j) /= result(i, result.columnCount() - 1);
        }
        result(i, result.columnCount() - 1) = 1.0;
    }
    return result;
}

}
