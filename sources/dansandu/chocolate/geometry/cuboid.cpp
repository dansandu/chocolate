#include "dansandu/chocolate/geometry/cuboid.hpp"

namespace dansandu::chocolate::geometry::cuboid
{

std::pair<Vertices, Triangles> generateCuboid(const float length, const float height, const float width)
{
    const auto l = length / 2.0f;
    const auto h = height / 2.0f;
    const auto w = width / 2.0f;
    return {Vertices{{{l, h, w, 1.0},
                      {-l, h, w, 1.0},
                      {-l, -h, w, 1.0},
                      {l, -h, w, 1.0},
                      {l, h, -w, 1.0},
                      {-l, h, -w, 1.0},
                      {-l, -h, -w, 1.0},
                      {l, -h, -w, 1.0}}},
            Triangles{{{2, 0, 1},
                       {2, 3, 0},
                       {1, 0, 4},
                       {1, 4, 5},
                       {7, 5, 4},
                       {7, 6, 5},
                       {3, 2, 6},
                       {3, 6, 7},
                       {3, 4, 0},
                       {3, 7, 4},
                       {1, 5, 6},
                       {1, 6, 2}}}};
}

}
