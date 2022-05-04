#include "dansandu/chocolate/raster/triangle.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/common.hpp"

#include <string>

using Catch::Detail::Approx;
using dansandu::ballotin::string::format;
using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::image::Image;
using dansandu::chocolate::ConstantVector3View;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::raster::triangle::drawSolidTriangle;
using dansandu::chocolate::raster::triangle::drawWireframeTriangle;
using dansandu::math::matrix::close;

static void REQUIRE_IMAGE(const Image& actual, const std::string& fileName)
{
    const auto expectedImagePath = "resources/dansandu/chocolate/expected_" + fileName;
    if (!(actual == readBitmapFile(expectedImagePath)))
    {
        const auto actualImagePath = "target/actual_" + fileName;
        writeBitmapFile(actualImagePath, actual);
        FAIL(format("actual image does not match expected image ", expectedImagePath, " -- check ", actualImagePath,
                    " for comparison"));
    }
}

static void REQUIRE_VECTOR(const ConstantVector3View actual, const ConstantVector3View expected)
{
    const auto tolerance = 1.0e-4f;
    if (!close(actual, expected, tolerance))
    {
        FAIL(format("actual vector ", actual, " does not match expected vector ", expected));
    }
}

TEST_CASE("triangle")
{
    auto a = Vector3{};
    auto b = Vector3{};
    auto c = Vector3{};

    auto actual = Image{50, 50};

    const auto tolerance = 0.05;

    const auto solidShader = [&](auto vertex, auto u, auto v, auto w)
    {
        REQUIRE(u > -tolerance);

        REQUIRE(u < 1.0f + tolerance);

        REQUIRE(v > -tolerance);

        REQUIRE(v < 1.0f + tolerance);

        REQUIRE(w > -tolerance);

        REQUIRE(w < 1.0f + tolerance);

        REQUIRE(u + v + w == Approx(1.0f));

        REQUIRE_VECTOR(vertex, u * a + v * b + w * c);

        actual(vertex.x(), vertex.y()) = Colors::white;
    };

    const auto wireShader = [&](auto vertex, auto u, auto v, auto w)
    {
        REQUIRE(u > -tolerance);

        REQUIRE(u < 1.0f + tolerance);

        REQUIRE(v > -tolerance);

        REQUIRE(v < 1.0f + tolerance);

        REQUIRE(w > -tolerance);

        REQUIRE(w < 1.0f + tolerance);

        REQUIRE(u + v + w == Approx(1.0f));

        REQUIRE_VECTOR(vertex, u * a + v * b + w * c);

        actual(vertex.x(), vertex.y()) = actual(vertex.x(), vertex.y()) == Colors::black ? Colors::red : Colors::green;
    };

    SECTION("flat top triangle with bottom vertex to the left")
    {
        a = Vector3{{15.0, 0.0, 0.0}};
        b = Vector3{{49.0, 0.0, 0.0}};
        c = Vector3{{0.0, 49.0, 0.0}};

        drawSolidTriangle(a, b, c, solidShader);

        drawWireframeTriangle(a, b, c, wireShader);

        REQUIRE_IMAGE(actual, "flat_top_triangle_bottom_to_left.bmp");
    }

    SECTION("flat top triangle with bottom vertex to the right")
    {
        a = Vector3{{15.0, 0.0, 0.0}};
        b = Vector3{{0.0, 0.0, 0.0}};
        c = Vector3{{49.0, 49.0, 0.0}};

        drawSolidTriangle(a, b, c, solidShader);

        drawWireframeTriangle(a, b, c, wireShader);

        REQUIRE_IMAGE(actual, "flat_top_triangle_bottom_to_right.bmp");
    }

    SECTION("flat bottom tip to the left")
    {
        a = Vector3{{0.0, 0.0, 0.0}};
        b = Vector3{{25.0, 49.0, 0.0}};
        c = Vector3{{49.0, 49.0, 0.0}};

        drawSolidTriangle(a, b, c, solidShader);

        drawWireframeTriangle(a, b, c, wireShader);

        REQUIRE_IMAGE(actual, "flat_bottom_triangle_tip_to_the_left.bmp");
    }

    SECTION("flat bottom tip to the right")
    {
        a = Vector3{{49.0, 0.0, 0.0}};
        b = Vector3{{25.0, 49.0, 0.0}};
        c = Vector3{{0.0, 49.0, 0.0}};

        drawSolidTriangle(a, b, c, solidShader);

        drawWireframeTriangle(a, b, c, wireShader);

        REQUIRE_IMAGE(actual, "flat_bottom_triangle_tip_to_the_right.bmp");
    }

    SECTION("obtuse triangle")
    {
        a = Vector3{{0.0, 0.0, 0.0}};
        b = Vector3{{5.0, 19.0, 0.0}};
        c = Vector3{{49.0, 49.0, 0.0}};

        drawSolidTriangle(a, b, c, solidShader);

        drawWireframeTriangle(a, b, c, wireShader);

        REQUIRE_IMAGE(actual, "obtuse_triangle.bmp");
    }
}
