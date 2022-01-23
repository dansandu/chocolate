#include "dansandu/chocolate/raster/triangle.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/common.hpp"

#include <string>

using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::image::Image;
using dansandu::chocolate::Point2;
using dansandu::chocolate::raster::triangle::drawTriangle;
using dansandu::chocolate::raster::triangle::drawWireframeTriangle;

static void REQUIRE_IMAGE(const Image& actual, const std::string& fileName)
{
    const auto expectedImagePath = "resources/dansandu/chocolate/expected_" + fileName;
    if (!(actual == readBitmapFile(expectedImagePath)))
    {
        const auto actualImagePath = "target/actual_" + fileName;
        writeBitmapFile(actualImagePath, actual);
        FAIL("actual image does not match expected image " + expectedImagePath + " -- check " + actualImagePath +
             " for comparison");
    }
}

TEST_CASE("triangle")
{
    auto actual = Image{50, 50};

    auto shader = [&actual](auto point) { actual(point) = Colors::white; };

    auto wireframeShader = [&actual](auto point)
    { actual(point) = (actual(point) == Colors::black ? Colors::red : Colors::green); };

    SECTION("flat top triangle with bottom vertex to the left")
    {
        const auto a = Point2{{15, 0}};
        const auto b = Point2{{49, 0}};
        const auto c = Point2{{0, 49}};

        drawTriangle(a, b, c, shader);

        drawWireframeTriangle(a, b, c, wireframeShader);

        REQUIRE_IMAGE(actual, "flat_top_triangle_bottom_to_left.bmp");
    }

    SECTION("flat top triangle with bottom vertex to the right")
    {
        const auto a = Point2{{15, 0}};
        const auto b = Point2{{0, 0}};
        const auto c = Point2{{49, 49}};

        drawTriangle(a, b, c, shader);

        drawWireframeTriangle(a, b, c, wireframeShader);

        REQUIRE_IMAGE(actual, "flat_top_triangle_bottom_to_right.bmp");
    }

    SECTION("flat bottom tip to the left")
    {
        const auto a = Point2{{0, 0}};
        const auto b = Point2{{25, 49}};
        const auto c = Point2{{49, 49}};

        drawTriangle(a, b, c, shader);

        drawWireframeTriangle(a, b, c, wireframeShader);

        REQUIRE_IMAGE(actual, "flat_bottom_triangle_tip_to_the_left.bmp");
    }

    SECTION("flat bottom tip to the right")
    {
        const auto a = Point2{{49, 0}};
        const auto b = Point2{{25, 49}};
        const auto c = Point2{{0, 49}};

        drawTriangle(a, b, c, shader);

        drawWireframeTriangle(a, b, c, wireframeShader);

        REQUIRE_IMAGE(actual, "flat_bottom_triangle_tip_to_the_right.bmp");
    }

    SECTION("obtuse triangle")
    {
        const auto a = Point2{{0, 0}};
        const auto b = Point2{{5, 19}};
        const auto c = Point2{{49, 49}};

        drawTriangle(a, b, c, shader);

        drawWireframeTriangle(a, b, c, wireframeShader);

        REQUIRE_IMAGE(actual, "obtuse_triangle.bmp");
    }
}
