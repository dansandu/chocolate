#include "dansandu/chocolate/raster/polygon.hpp"
#include "dansandu/chocolate/interpolation.hpp"
#include "dansandu/chocolate/raster/triangle_tracer.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::chocolate::interpolation::BilinearInterpolation;
using dansandu::chocolate::raster::triangle_tracer::traceTriangle;

namespace dansandu::chocolate::raster::polygon
{

void drawPolygon(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                 const ConstantVector3View d, const bool wireframe, const ShaderType& shader)
{
    const auto bilinear = BilinearInterpolation{a, b, c, d};

    const auto callback = [&](const ConstantVector3View vertex)
    {
        const auto coordinates = bilinear(vertex);

        shader(vertex, coordinates.x(), coordinates.y(), coordinates.z(), coordinates.w());
    };

    traceTriangle(a, b, c, wireframe, callback);

    traceTriangle(c, d, a, wireframe, callback);
}

}
