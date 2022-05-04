#include "dansandu/chocolate/raster/triangle.hpp"
#include "dansandu/chocolate/interpolation.hpp"
#include "dansandu/chocolate/raster/line_tracer.hpp"
#include "dansandu/math/matrix.hpp"
#include "dansandu/math/permutation.hpp"

#include <algorithm>
#include <vector>

using dansandu::chocolate::interpolation::interpolate;
using dansandu::chocolate::raster::line_tracer::LineTracer;
using dansandu::math::matrix::dotProduct;
using dansandu::math::permutation::getIdentityPermutation;

namespace dansandu::chocolate::raster::triangle
{

static std::vector<int> getYxOrderPermutation(const std::vector<Point2>& points)
{
    auto permutation = getIdentityPermutation(points.size());

    const auto comparator = [&points](const auto a, const auto b)
    { return (points[a].y() < points[b].y()) | ((points[a].y() == points[b].y()) & (points[a].x() < points[b].x())); };

    std::sort(permutation.begin(), permutation.end(), comparator);

    return permutation;
}

static Point2 getRounded(const ConstantVector3View vector)
{
    return Point2{{static_cast<int>(std::round(vector.x())), static_cast<int>(std::round(vector.y()))}};
}

// order of vertices/points must be the initial order passed to the draw function
// order of permutation must be tip, left, right
// order of function callback barycentric coordinates are the order of vertices/points
static void drawFlatTriangle(const std::vector<Vector3>& vertices, const std::vector<Point2>& points,
                             const std::vector<int>& permutation,
                             const std::function<void(ConstantVector3View, float, float, float)>& shader)
{
    const auto v0 = vertices[1] - vertices[0];
    const auto v1 = vertices[2] - vertices[0];

    const auto d00 = dotProduct(v0, v0);
    const auto d01 = dotProduct(v0, v1);
    const auto d11 = dotProduct(v1, v1);
    const auto dnm = d00 * d11 - d01 * d01;

    const auto& tip = points[permutation[0]];
    const auto& left = points[permutation[1]];
    const auto& right = points[permutation[2]];

    const auto increasing = tip.y() < left.y();
    const auto dy = increasing - !increasing;

    auto leftTracer = LineTracer{tip, left};
    auto rightTracer = LineTracer{tip, right};

    for (auto y = tip.y(); y != left.y() + dy; y += dy)
    {
        const auto leftXbegin = leftTracer.position().x();
        auto leftXend = leftXbegin;

        while (leftTracer.step() & (y == leftTracer.position().y()))
        {
            leftXend = leftTracer.position().x();
        }

        const auto rightXbegin = rightTracer.position().x();
        auto rightXend = rightXbegin;

        while (rightTracer.step() & (y == rightTracer.position().y()))
        {
            rightXend = rightTracer.position().x();
        }

        const auto minimum = std::min(leftXbegin, leftXend);
        const auto maximum = std::max(rightXbegin, rightXend);

        for (auto x = minimum; x <= maximum; ++x)
        {
            const auto tolerance = 1.0e-5f;
            const auto vertex = interpolate(vertices[permutation[0]], vertices[permutation[1]],
                                            vertices[permutation[2]], x, y, tolerance);

            const auto v2 = vertex - vertices[0];

            const auto d20 = dotProduct(v2, v0);
            const auto d21 = dotProduct(v2, v1);

            const auto v = (d11 * d20 - d01 * d21) / dnm;
            const auto w = (d00 * d21 - d01 * d20) / dnm;
            const auto u = 1.0f - v - w;

            shader(vertex, u, v, w);
        }
    }
}

void drawSolidTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                       const std::function<void(ConstantVector3View, float, float, float)>& shader)
{
    auto vertices = std::vector<Vector3>{{static_cast<Vector3>(a), static_cast<Vector3>(b), static_cast<Vector3>(c)}};
    auto points = std::vector<Point2>{{getRounded(a), getRounded(b), getRounded(c)}};

    auto permutation = getYxOrderPermutation(points);

    if (points[permutation[0]].y() != points[permutation[1]].y() &&
        points[permutation[1]].y() != points[permutation[2]].y())
    {
        const auto x = (vertices[permutation[1]].y() - vertices[permutation[0]].y()) *
                           (vertices[permutation[2]].x() - vertices[permutation[0]].x()) /
                           (vertices[permutation[2]].y() - vertices[permutation[0]].y()) +
                       vertices[permutation[0]].x();

        const auto tolerance = 1.0e-5f;
        const auto d =
            interpolate(vertices[permutation[0]], vertices[permutation[2]], x, vertices[permutation[1]].y(), tolerance);

        vertices.push_back(d);
        points.push_back(getRounded(d));
        permutation.push_back(3);

        std::swap(permutation[2], permutation[3]);

        if (points[permutation[2]].x() < points[permutation[1]].x())
        {
            std::swap(permutation[2], permutation[1]);
        }

        drawFlatTriangle(vertices, points, {permutation[0], permutation[1], permutation[2]}, shader);

        drawFlatTriangle(vertices, points, {permutation[3], permutation[1], permutation[2]}, shader);
    }
    else if (points[permutation[0]].y() == points[permutation[1]].y())
    {
        drawFlatTriangle(vertices, points, {permutation[2], permutation[0], permutation[1]}, shader);
    }
    else
    {
        drawFlatTriangle(vertices, points, {permutation[0], permutation[1], permutation[2]}, shader);
    }
}

void drawWireframeTriangle(const ConstantVector3View a, const ConstantVector3View b, const ConstantVector3View c,
                           const std::function<void(ConstantVector3View, float, float, float)>& shader)
{
    const auto v0 = b - a;
    const auto v1 = c - a;

    const auto d00 = dotProduct(v0, v0);
    const auto d01 = dotProduct(v0, v1);
    const auto d11 = dotProduct(v1, v1);
    const auto dnm = d00 * d11 - d01 * d01;

    const auto drawLine = [&](const ConstantVector3View start, const ConstantVector3View end)
    {
        auto lineTracer = LineTracer{getRounded(start), getRounded(end)};
        do
        {
            const auto point = lineTracer.position();

            const auto tolerance = 1.0e-5f;
            const auto vertex = interpolate(start, end, point.x(), point.y(), tolerance);

            const auto v2 = vertex - a;

            const auto d20 = dotProduct(v2, v0);
            const auto d21 = dotProduct(v2, v1);

            const auto v = (d11 * d20 - d01 * d21) / dnm;
            const auto w = (d00 * d21 - d01 * d20) / dnm;
            const auto u = 1.0f - v - w;

            shader(vertex, u, v, w);
        } while (lineTracer.step());
    };

    drawLine(a, b);
    drawLine(b, c);
    drawLine(c, a);
}

}
