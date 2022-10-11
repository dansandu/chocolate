#include "dansandu/chocolate/transform.hpp"
#include "dansandu/math/common.hpp"

using dansandu::math::common::close;
using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::dotProduct;
using dansandu::math::matrix::normalized;

namespace dansandu::chocolate::transform
{

Matrix4 scale(const float x, const float y, const float z)
{
    // clang-format off
    return Matrix4{{{   x, 0.0f, 0.0f, 0.0f},
                    {0.0f,    y, 0.0f, 0.0f},
                    {0.0f, 0.0f,    z, 0.0f},
                    {0.0f, 0.0f, 0.0f, 1.0f}}};
    // clang-format on
}

Matrix4 translate(const float x, const float y, const float z)
{
    // clang-format off
    return Matrix4{{{1.0f, 0.0f, 0.0f, 0.0f},
                    {0.0f, 1.0f, 0.0f, 0.0f},
                    {0.0f, 0.0f, 1.0f, 0.0f},
                    {   x,    y,    z, 1.0f}}};
    // clang-format on
}

Matrix4 translate(const ConstantVector3View vector)
{
    return translate(vector.x(), vector.y(), vector.z());
}

Matrix4 rotateByX(const float radians)
{
    const auto cos = std::cos(radians);
    const auto sin = std::sin(radians);
    // clang-format off
    return Matrix4{{{1.0f,   0.0f,  0.0f, 0.0f},
                    {0.0f,    cos,   sin, 0.0f},
                    {0.0f,   -sin,   cos, 0.0f},
                    {0.0f,   0.0f,  0.0f, 1.0f}}};
    // clang-format on
}

Matrix4 rotateByY(const float radians)
{
    const auto cos = std::cos(radians);
    const auto sin = std::sin(radians);
    // clang-format off
    return Matrix4{{{ cos, 0.0f, -sin, 0.0f},
                    {0.0f, 1.0f, 0.0f, 0.0f},
                    { sin, 0.0f,  cos, 0.0f},
                    {0.0f, 0.0f, 0.0f, 1.0f}}};
    // clang-format on
}

Matrix4 rotateByZ(const float radians)
{
    const auto cos = std::cos(radians);
    const auto sin = std::sin(radians);
    // clang-format off
    return Matrix4{{{ cos,   sin, 0.0f, 0.0f},
                    {-sin,   cos, 0.0f, 0.0f},
                    {0.0f,  0.0f, 1.0f, 0.0f},
                    {0.0f,  0.0f, 0.0f, 1.0f}}};
    // clang-format on
}

Matrix4 lookAt(const ConstantVector3View eye, const ConstantVector3View target, const ConstantVector3View up)
{
    const auto w = normalized(eye - target);
    const auto u = normalized(crossProduct(up, w));
    const auto v = crossProduct(w, u);
    // clang-format off
    return translate(-eye) * Matrix4{{{u.x(), v.x(), w.x(), 0.0f},
                                      {u.y(), v.y(), w.y(), 0.0f},
                                      {u.z(), v.z(), w.z(), 0.0f},
                                      { 0.0f,  0.0f,  0.0f, 1.0f}}};
    // clang-format on
}

Matrix4 perspective(const float near, const float far, const float fieldOfViewRadians, const float aspect)
{
    const auto ctg = 1.0f / std::tan(0.5f * fieldOfViewRadians);
    const auto p33 = (far + near) / (near - far);
    const auto p43 = 2.0f * far * near / (near - far);
    // clang-format off
    return Matrix4{{{ ctg,         0.0f, 0.0f,  0.0f}, 
                    {0.0f, aspect * ctg, 0.0f,  0.0f},
                    {0.0f,         0.0f,  p33, -1.0f},
                    {0.0f,         0.0f,  p43,  0.0f}}};
    // clang-format on
}

Matrix4 viewport(const float width, const float height)
{
    const auto x = width / 2.0f;
    const auto y = height / 2.0f;
    return scale(x, -y, 1.0f) * translate(x, y, -1.0f);
}

Vertices dehomogenized(const ConstantVerticesView vertices)
{
    auto result = static_cast<Vertices>(vertices);
    for (auto i = 0; i < result.rowCount(); ++i)
    {
        for (auto j = 0; j < result.columnCount(); ++j)
        {
            result(i, j) /= result(i, result.columnCount() - 1);
        }
    }
    return result;
}

}
