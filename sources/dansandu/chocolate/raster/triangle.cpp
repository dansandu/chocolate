#include "dansandu/chocolate/raster/triangle.hpp"
#include "dansandu/chocolate/interpolation.hpp"
#include "dansandu/chocolate/raster/line_tracer.hpp"
#include "dansandu/math/matrix.hpp"

#include <algorithm>
#include <vector>

using dansandu::chocolate::interpolation::interpolate;
using dansandu::chocolate::raster::line_tracer::LineTracer;
using dansandu::math::matrix::close;
using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::dotProduct;
using dansandu::math::matrix::Slicer;

namespace dansandu::chocolate::raster::triangle
{

static std::vector<int> getYxOrderPermutation(const std::vector<Point2>& points)
{
    auto permutation = std::vector<int>{};
    permutation.reserve(points.size());
    for (decltype(points.size()) i = 0; i < points.size(); ++i)
    {
        permutation.push_back(i);
    }

    const auto comparator = [&points](const auto a, const auto b)
    { 
        return (points[a].y() < points[b].y()) | ((points[a].y() == points[b].y()) & (points[a].x() < points[b].x())); 
    }

    std::sort(permutation.begin(), permutation.end(), comparator);

    return permutation;
}

static Point2 getRounded(const ConstantVector3View vector)
{
    return Point2{{static_cast<int>(std::round(vector.x())), static_cast<int>(std::round(vector.y()))}};
}

static void drawFlatTriangle(const Vertex3& a, const Vertex3& b, const Vertex3& c, 
                             const Point2& tip, const Point2& left, const Point& right,
                             const std::vector<int>& permutation,
                             const std::function<void(ConstantVector3View, float, float, float)>& shader)
{
    const auto v0 = b - a;
    const auto v1 = c - a;

    const auto d00 = dotProduct(v0, v0);
    const auto d01 = dotProduct(v0, v1);
    const auto d11 = dotProduct(v1, v1);
    const auto dnm = d00 * d11 - d01 * d01;

    const auto increasing = tip.y() < left.y();
    const auto dy = increasing - !increasing;

    auto leftTracer = LineTracer{tip, left};
    auto rightTracer = LineTracer{tip, right};

    const auto inverted = getInvertedPermutation(permutation);

    for (auto y = tip.y(); y != left.y() + dy; y += dy)
    {
        const auto leftXbegin = leftTracer.position().x();
        auto leftXend = leftXbegin;

        const auto rightXbegin = rightTracer.position().x();
        auto rightXend = rightXbegin;

        while (leftTracer.step() & (y == leftTracer.position().y()))
        {
            leftXend = leftTracer.position().x();
        }

        while (rightTracer.step() & (y == rightTracer.position().y()))
        {
            rightXend = rightTracer.position().x();
        }

        for (auto x = std::min(leftXbegin, leftXend); x <= std::max(rightXbegin, rightXend); ++x)
        {
            const auto vertex = interpolate(tip, left, right, x, y);

            const auto v2 = vertex - a;

            const auto d20 = dotProduct(v2, v0);
            const auto d21 = dotProduct(v2, v1);

            const auto v = (d11 * d20 - d01 * d21) / dnm;
            const auto w = (d00 * d21 - d01 * d20) / dnm;
            const auto u = 1.0f - v - w;

            const auto coordinates = {u, v, w};

            shader(vertex, coordinates[permutation[0]], coordinates[permutation[1]], coordinates[permutation[2]]);
        }
    }
}

void drawTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                  const std::function<void(ConstantVector3View, float, float, float)>& shader)
{
    auto points = std::vector<Point2>{{getRounded(a), getRounded(b), getRounded(c)}};
    auto vertices = std::vector<Vector3>{{static_cast<Vector3>(a), static_cast<Vector3>(b), static_cast<Vector3>(c)}};

    auto permutation = getYxOrderPermutation(points);

    if ((points[permutation[0]].y() != points[permutation[1]].y()) &
        (points[permutation[1]].y() != points[permutation[2]].y()))
    {
        const auto x = (vertices[permutation[1]].y() - vertices[permutation[0]].y()) *
                           (vertices[permutation[2]].x() - vertices[permutation[0]].x()) /
                           (vertices[permutation[2]].y() - vertices[permutation[0]].y()) +
                       vertices[permutation[0]].x();

        const auto d = interpolate(vertices[permutation[0]], vertices[permutation[2]], x, vertices[permutation[1]].y());
        
        vertices.push_back(d);
        points.push_back(static_cast<Vector3>(d));
        permutation.push_back(3);

        if (points[permutation[3]].x() < points[permutation[1]].x())
        {
            std::swap(permutation[3], permutation[1]);
        }

        drawFlatTriangle(vertices[permutation[0]], vertices[permutation[1]], vertices[permutation[2]], 
                         points[permutation[0]], points[permutation[1]], points[permutation[2]],
                         permutation, shader);


        drawFlatTriangle(c, b, middle, shader);
    }
    else if (a.y() == b.y())
    {
        drawFlatTriangle(c, a, b, shader);
    }
    else
    {
        drawFlatTriangle(a, b, c, shader);
    }
}

static void drawLine(const ConstantVector3View a, const ConstantVector3View b,
                     const std::function<void(ConstantVector3View, float, float, float)>& shader)
{
    const auto round = [](auto v) {
        return Point2{{static_cast<int>(std::round(v.x())), static_cast<int>(std::round(v.y()))}};
    };

    auto lineTracer = LineTracer{round(a), round(b)};
    do
    {
        const auto point = lineTracer.position();
        const auto vector = Vector3{{static_cast<float>(point.x()), static_cast<float>(point.y()), 0.0f}};
        shader(vector, 0.0f, 0.0f, 0.0f);
    } while (lineTracer.step());
}

void drawWireframeTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                           const std::function<void(ConstantVector3View, float, float, float)>& shader)
{
    drawLine(a, b, shader);
    drawLine(b, c, shader);
    drawLine(c, a, shader);
}

}
