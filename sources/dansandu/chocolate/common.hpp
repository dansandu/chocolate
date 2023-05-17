#pragma once

#include "dansandu/ballotin/logging.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/math/matrix.hpp"

namespace dansandu::chocolate
{

using dansandu::math::matrix::dynamic;

using ConstantPoint2View = dansandu::math::matrix::ConstantMatrixView<int, 1, 2>;

using Point2 = dansandu::math::matrix::Matrix<int, 1, 2>;

using Matrix4 = dansandu::math::matrix::Matrix<float, 4, 4>;

using Vector3 = dansandu::math::matrix::Matrix<float, 1, 3>;

using Vector4 = dansandu::math::matrix::Matrix<float, 1, 4>;

using Vertices = dansandu::math::matrix::Matrix<float, dynamic, 4>;

using Triangles = dansandu::math::matrix::Matrix<int, dynamic, 3>;

using Normals = dansandu::math::matrix::Matrix<float, dynamic, 3>;

using TextureMapping = dansandu::math::matrix::Matrix<float, dynamic, 2>;

using ConstantMatrix4View = dansandu::math::matrix::ConstantMatrixView<float, 4, 4>;

using ConstantVector3View = dansandu::math::matrix::ConstantMatrixView<float, 1, 3>;

using ConstantVerticesView = dansandu::math::matrix::ConstantMatrixView<float, dynamic, 4>;

using ConstantTrianglesView = dansandu::math::matrix::ConstantMatrixView<int, dynamic, 3>;

using ConstantNormalsView = dansandu::math::matrix::ConstantMatrixView<float, dynamic, 3>;

using ConstantTextureMappingView = dansandu::math::matrix::ConstantMatrixView<float, dynamic, 2>;

using Vector3Slicer = dansandu::math::matrix::Slicer<dynamic, 0, 1, 3>;

using dansandu::math::matrix::sliceRow;

inline Vector3 toVector3(const dansandu::canvas::color::Color color)
{
    return Vector3{{color.red() / 255.0f, color.green() / 255.0f, color.blue() / 255.0f}};
}

inline dansandu::canvas::color::Color toColor(const ConstantVector3View vector)
{
    return dansandu::canvas::color::Color(std::min(255.0f, std::max(0.0f, 255.0f * vector.x())),
                                          std::min(255.0f, std::max(0.0f, 255.0f * vector.y())),
                                          std::min(255.0f, std::max(0.0f, 255.0f * vector.z())));
}

template<typename T, int M, int N, dansandu::math::matrix::DataStorageStrategy S>
dansandu::math::matrix::Matrix<int, M, N>
getRounded(const dansandu::math::matrix::MatrixImplementation<T, M, N, S>& matrix)
{
    auto result = dansandu::math::matrix::Matrix<int, M, N>{matrix.rowCount(), matrix.columnCount()};
    auto source = matrix.cbegin();
    auto target = result.begin();
    while (source != matrix.cend() && target != result.end())
    {
        *target++ = static_cast<int>(std::round(*source++));
    }
    return result;
}

}
