#include "dansandu/chocolate/interpolation.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/chocolate/common.test.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::ballotin::string::format;
using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::image::Image;
using dansandu::chocolate::checkImage;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::interpolation::BarycentricCoordinates;
using dansandu::chocolate::interpolation::interpolate;
using dansandu::math::close;
using dansandu::radiance::Tolerance;

TEST_CASE("interpolation")
{
    SECTION("line")
    {
        const auto epsilon = 1.0e-5;

        // clang-format off
        const auto a = Vector3{{  2.0, -10.0,   20.0}};
        const auto b = Vector3{{-50.0,  80.0, -100.0}};
        const auto t = 0.25;
        // clang-format on

        const auto expected = a + (b - a) * t;
        const auto actual = interpolate(a, b, expected.x(), expected.y(), epsilon);

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("plane")
    {
        const auto epsilon = 1.0e-4;

        // clang-format off
        const auto a = Vector3{{  2.0, -10.0,   20.0}};
        const auto b = Vector3{{-50.0,  80.0, -100.0}};
        const auto c = Vector3{{ 33.0,  10.0,  -77.0}};
        // clang-format on

        const auto u = 0.20;
        const auto v = 0.70;

        const auto expected = c + (a - c) * u + (b - c) * v;
        const auto actual = interpolate(a, b, c, expected.x(), expected.y(), epsilon);

        REQUIRE(actual == Tolerance(expected));
    }

    SECTION("barycentric coordinates")
    {
        const auto epsilon = 1.0e-4;

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
                const auto point = Vector3{{static_cast<double>(x), static_cast<double>(y), 0.0}};
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

        REQUIRE(checkImage(image, "barycentric.bmp"));
    }
}
