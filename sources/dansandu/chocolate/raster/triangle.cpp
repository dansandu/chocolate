#include "dansandu/chocolate/raster/triangle.hpp"
#include "dansandu/chocolate/raster/line_tracer.hpp"

using dansandu::chocolate::raster::line_tracer::LineTracer;

namespace dansandu::chocolate::raster::triangle
{

static void drawFlatTriangle(const ConstantPoint2View tip, const ConstantPoint2View left,
                             const ConstantPoint2View right, const std::function<void(ConstantPoint2View)>& shader)
{
    const auto increasing = tip.y() < left.y();
    const auto dy = increasing - !increasing;

    auto leftTracer = LineTracer{tip, left};
    auto rightTracer = LineTracer{tip, right};

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
            auto point = Point2{{x, y}};
            shader(point);
        }
    }
}

static bool smallerThanYxOrder(const ConstantPoint2View a, const ConstantPoint2View b)
{
    return (a.y() < b.y()) | ((a.y() == b.y()) & (a.x() < b.x()));
}

static void sortPointsByYxAndMirror(Point2& top, Point2& left, Point2& right)
{
    using std::swap;
    if (smallerThanYxOrder(top, left))
    {
        if (smallerThanYxOrder(right, top))
        {
            swap(top, right);
        }
    }
    else
    {
        if (smallerThanYxOrder(left, right))
        {
            swap(top, left);
        }
        else
        {
            swap(top, right);
        }
    }

    if (smallerThanYxOrder(right, left))
    {
        swap(left, right);
    }
}

void drawTriangle(const ConstantPoint2View a, const ConstantPoint2View b, const ConstantPoint2View c,
                  const std::function<void(ConstantPoint2View)>& shader)
{
    using std::swap;

    auto top = static_cast<Point2>(a);
    auto left = static_cast<Point2>(b);
    auto right = static_cast<Point2>(c);

    sortPointsByYxAndMirror(top, left, right);

    if (top.y() != left.y() && left.y() != right.y())
    {
        const auto x = (left.y() - top.y()) * (right.x() - top.x()) / (right.y() - top.y()) + top.x();
        auto middle = Point2{{x, left.y()}};

        if (middle.x() < left.x())
        {
            swap(left, middle);
        }

        drawFlatTriangle(top, left, middle, shader);
        drawFlatTriangle(right, left, middle, shader);
    }
    else if (top.y() == left.y())
    {
        drawFlatTriangle(right, top, left, shader);
    }
    else
    {
        drawFlatTriangle(top, left, right, shader);
    }
}

static void drawLine(const ConstantPoint2View a, const ConstantPoint2View b,
                     const std::function<void(ConstantPoint2View)>& shader)
{
    auto lineTracer = LineTracer{a, b};
    do
    {
        shader(lineTracer.position());
    } while (lineTracer.step());
}

void drawWireframeTriangle(const ConstantPoint2View a, const ConstantPoint2View b, const ConstantPoint2View c,
                           const std::function<void(ConstantPoint2View)>& shader)
{
    drawLine(a, b, shader);
    drawLine(b, c, shader);
    drawLine(c, a, shader);
}

}
