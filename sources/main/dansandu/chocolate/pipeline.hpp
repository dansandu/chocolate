#pragma once

#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::pipeline
{

class Pipeline
{
public:
    Pipeline();

    Pipeline& culling(const bool enabled);

    Pipeline& clipping(const bool enabled);

    Pipeline& perspectiveProjection(const double near, const double far, const double fieldOfViewRadians,
                                    const double aspect);

    void renderWireframe(const ConstantVerticesView vertices, const ConstantTrianglesView triangles,
                         const ConstantMatrix4View& transformation, const dansandu::canvas::color::Color color,
                         dansandu::canvas::image::Image& image) const;

private:
    bool cull_;
    bool clip_;
    Matrix4 projection_;
};

}
