#include "dansandu/chocolate/pipeline.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/geometry/clipping.hpp"
#include "dansandu/chocolate/raster/drawing.hpp"
#include "dansandu/chocolate/transformation.hpp"

using dansandu::canvas::color::Color;
using dansandu::canvas::image::Image;
using dansandu::chocolate::geometry::clipping::clip;
using dansandu::chocolate::geometry::clipping::cull;
using dansandu::chocolate::raster::drawing::drawWireframe;

using namespace dansandu::chocolate::transformation;

namespace dansandu::chocolate::pipeline
{

static constexpr auto defaultPerspectiveNear = 1.0f;
static constexpr auto defaultPerspectiveFar = 1000.0f;
static constexpr auto defaultPerspectiveFieldOfView = 1.92f;
static constexpr auto defaultPerspectiveAspect = 1.0f;

Pipeline::Pipeline()
    : cull_{false},
      clip_{false},
      projection_{transposed(perspective(defaultPerspectiveNear, defaultPerspectiveFar, defaultPerspectiveFieldOfView,
                                         defaultPerspectiveAspect))}
{
}

Pipeline& Pipeline::culling(const bool enabled)
{
    cull_ = enabled;
    return *this;
}

Pipeline& Pipeline::clipping(const bool enabled)
{
    clip_ = enabled;
    return *this;
}

Pipeline& Pipeline::perspectiveProjection(const float near, const float far, const float fieldOfViewRadians,
                                          const float aspect)
{
    projection_ = transposed(perspective(near, far, fieldOfViewRadians, aspect));
    return *this;
}

void Pipeline::renderWireframe(const ConstantVerticesView vertices, const ConstantTrianglesView triangles,
                               const ConstantMatrix4View& transformation, const Color color, Image& image) const
{
    auto tVertices = vertices * transposed(transformation);
    auto tTriangles = Triangles{triangles};
    auto normals = Normals{};

    if (cull_)
    {
        auto result = cull(tVertices, tTriangles);
        tTriangles = std::move(result.first);
        normals = std::move(result.second);
    }

    tVertices = tVertices * projection_;

    if (clip_)
    {
        auto result = clip(tVertices, tTriangles, normals);
        tVertices = std::move(std::get<0>(result));
        tTriangles = std::move(std::get<1>(result));
    }

    tVertices = dehomogenized(tVertices) * transposed(viewport(image.width(), image.height()));

    drawWireframe(tVertices, tTriangles, color, image);
}

}
