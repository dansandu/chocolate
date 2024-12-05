#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::geometry::cuboid
{

std::pair<Vertices, Triangles> generateTriangleMeshCuboid(const double length, const double height, const double width);

std::pair<Vertices, Polygons> generatePolygonMeshCuboid(const double length, const double height, const double width);

}
