#include "dansandu/chocolate/interpolation.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/chocolate/common.hpp"

using dansandu::ballotin::string::format;
using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::image::Image;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::interpolation::BarycentricCoordinates;
using dansandu::chocolate::interpolation::interpolate;
using dansandu::math::matrix::close;

static void REQUIRE_IMAGE(const Image& actualImage, const std::string& fileName)
{
    const auto expectedImagePath = "resources/dansandu/chocolate/expected_" + fileName;
    const auto expectedImage = readBitmapFile(expectedImagePath);
    if (actualImage != expectedImage)
    {
        const auto actualImagePath = "target/actual_" + fileName;
        writeBitmapFile(actualImagePath, actualImage);
        FAIL(format("actual image does not match expected image ", expectedImagePath, " -- check ", actualImagePath,
                    " for comparison"));
    }
    else
    {
        SUCCEED(format("the image ", fileName, " matches ", expectedImagePath));
    }
}

TEST_CASE("interpolation")
{
    SECTION("line")
    {
        const auto epsilon = 1.0e-5f;

        // clang-format off
        const auto a = Vector3{{  2.0f, -10.0f,   20.0f}};
        const auto b = Vector3{{-50.0f,  80.0f, -100.0f}};
        const auto t = 0.25f;
        // clang-format on

        const auto expected = a + (b - a) * t;
        const auto actual = interpolate(a, b, expected.x(), expected.y(), epsilon);

        REQUIRE(close(actual, expected, epsilon));
    }

    SECTION("plane")
    {
        const auto epsilon = 1.0e-4f;

        // clang-format off
        const auto a = Vector3{{  2.0f, -10.0f,   20.0f}};
        const auto b = Vector3{{-50.0f,  80.0f, -100.0f}};
        const auto c = Vector3{{ 33.0f,  10.0f,  -77.0f}};
        // clang-format on

        const auto u = 0.20f;
        const auto v = 0.70f;

        const auto expected = c + (a - c) * u + (b - c) * v;
        const auto actual = interpolate(a, b, c, expected.x(), expected.y(), epsilon);

        REQUIRE(close(actual, expected, epsilon));
    }

    SECTION("barycentric coordinates")
    {
        const auto epsilon = 1.0e-4f;

        const auto width = 150;
        const auto height = 100;

        // clang-format off
        const auto a = Vector3{{ 40.0,  15.0, 0.0}};
        const auto b = Vector3{{  5.0,  90.0, 0.0}};
        const auto c = Vector3{{125.0,  70.0, 0.0}};
        // clang-format on

        const auto barycentric = BarycentricCoordinates{a, b, c};

        auto goodCoordinates = true;

        auto image = Image{width, height};

        for (auto y = 0; y < height; ++y)
        {
            for (auto x = 0; x < width; ++x)
            {
                const auto point = Vector3{{static_cast<float>(x), static_cast<float>(y), 0.0}};
                const auto coordinates = barycentric(point);
                const auto samePoint = coordinates(0) * a + coordinates(1) * b + coordinates(2) * c;

                goodCoordinates = goodCoordinates && close(point, samePoint, epsilon);

                if (std::all_of(coordinates.cbegin(), coordinates.cend(),
                                [](const auto& c) { return 0.0 <= c && c <= 1.0; }))
                {
                    image(x, y) = Colors::white;
                }
            }
        }

        image(a.x(), a.y()) = Colors::red;
        image(b.x(), b.y()) = Colors::red;
        image(c.x(), c.y()) = Colors::red;

        REQUIRE(goodCoordinates);

        REQUIRE_IMAGE(image, "barycentric.bmp");
    }
}
