#pragma once

#include "dansandu/math/matrix.hpp"

namespace dansandu::chocolate
{

using dansandu::math::matrix::dynamic;

using ConstantPoint2View = dansandu::math::matrix::ConstantMatrixView<int, 1, 2>;

using Point2 = dansandu::math::matrix::Matrix<int, 1, 2>;

using Matrix4 = dansandu::math::matrix::Matrix<float, 4, 4>;

using Vector3 = dansandu::math::matrix::Matrix<float, 1, 3>;

using Vector4 = dansandu::math::matrix::Matrix<float, 1, 4>;

using Vector3View = dansandu::math::matrix::MatrixView<float, 1, 3>;

using ConstantVector3View = dansandu::math::matrix::ConstantMatrixView<float, 1, 3>;

using ConstantVerticesView = dansandu::math::matrix::ConstantMatrixView<float, dynamic, 4>;

using ConstantTrianglesView = dansandu::math::matrix::ConstantMatrixView<int, dynamic, 3>;

using Vertices = dansandu::math::matrix::Matrix<float, dynamic, 4>;

using Triangles = dansandu::math::matrix::Matrix<int, dynamic, 3>;

}
