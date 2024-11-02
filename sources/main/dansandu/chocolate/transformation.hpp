#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::transformation
{

Matrix4 shearX(const double y, const double z);

Matrix4 scale(const double x, const double y, const double z);

Matrix4 translate(const double x, const double y, const double z);

Matrix4 translate(const ConstantVector3View vector);

Matrix4 rotateByX(const double radians);

Matrix4 rotateByY(const double radians);

Matrix4 rotateByZ(const double radians);

Matrix4 lookAt(const ConstantVector3View eye, const ConstantVector3View target, const ConstantVector3View up);

Matrix4 perspective(const double near, const double far, const double fieldOfViewRadians, const double aspect);

Matrix4 viewport(const double width, const double height);

Vertices dehomogenized(const ConstantVerticesView vertices);

}
