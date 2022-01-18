#include "dansandu/chocolate/raster/line_tracer.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/common.hpp"

#include <cmath>

using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::image::Image;
using dansandu::chocolate::Point2;
using dansandu::chocolate::raster::line_tracer::LineTracer;
using dansandu::math::common::pi;

static int toInt(const float value)
{
    return static_cast<int>(std::round(value));
}

static void plotInbetween(Image& image, const Point2 point)
{
    if (auto& color = image(point); color == Colors::black)
    {
        color = Colors::white;
    }
    else
    {
        color = Colors::red;
    }
}

static void plotEnd(Image& image, const Point2 point)
{
    if (auto& color = image(point); color == Colors::white)
    {
        color = Colors::green;
    }
    else
    {
        color = Colors::red;
    }
}

TEST_CASE("line_tracer")
{
    const auto width = 151;
    const auto height = 151;
    const auto lineCount = 24;
    const auto startLength = 12.0f;
    const auto endLength = 75.0f;
    const auto center = Point2{{width / 2, height / 2}};

    auto actual = Image{width, height};
    for (auto i = 0; i < lineCount; ++i)
    {
        const auto angle = i * 2.0f * pi<float> / lineCount;
        const auto sin = std::sin(angle);
        const auto cos = std::cos(angle);

        const auto start = center + Point2{{toInt(cos * startLength), toInt(sin * startLength)}};
        const auto end = center + Point2{{toInt(cos * endLength), toInt(sin * endLength)}};

        auto lineTracer = LineTracer{start, end};

        do
        {
            plotInbetween(actual, lineTracer.position());
        } while (lineTracer.step());

        plotEnd(actual, start);
        plotEnd(actual, end);
    }

    const auto expected = readBitmapFile("resources/dansandu/chocolate/expected_line_tracer.bmp");
    const auto success = expected == actual;
    if (!success)
    {
        writeBitmapFile("target/actual_line_tracer.bmp", actual);
    }
    REQUIRE(success);
}
