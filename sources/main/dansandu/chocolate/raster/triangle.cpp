#include "dansandu/chocolate/raster/triangle.hpp"
#include "dansandu/chocolate/interpolation.hpp"
#include "dansandu/chocolate/raster/triangle_tracer.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::chocolate::interpolation::BarycentricCoordinates;
using dansandu::chocolate::raster::triangle_tracer::traceTriangle;

namespace dansandu::chocolate::raster::triangle
{

void drawTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                  const bool wireframe, const ShaderType& shader)
{
    const auto barycentric = BarycentricCoordinates{a, b, c};

    const auto callback = [&](const ConstantVector3View vertex)
    {
        const auto coordinates = barycentric(vertex);

        shader(vertex, coordinates.x(), coordinates.y(), coordinates.z());
    };

    traceTriangle(a, b, c, wireframe, callback);
}

}
