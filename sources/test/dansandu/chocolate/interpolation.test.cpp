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
using dansandu::chocolate::between;
using dansandu::chocolate::checkImage;
using dansandu::chocolate::ConstantTextureMappingView;
using dansandu::chocolate::getRounded;
using dansandu::chocolate::TextureMapping;
using dansandu::chocolate::Vector2;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::interpolation::BarycentricCoordinates;
using dansandu::chocolate::interpolation::BilinearInterpolation;
using dansandu::chocolate::interpolation::canInterpolateBilineary;
using dansandu::chocolate::interpolation::interpolate;
using dansandu::chocolate::interpolation::isConvexPolygon;
using dansandu::chocolate::interpolation::Line;
using dansandu::math::close;
using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Slicer;
using dansandu::radiance::Tolerance;

static void rasterizeTexture(const Image& texture, const ConstantTextureMappingView textureMapping,
                             const BilinearInterpolation& interpolation, Image& screen)
{
    const auto tex = [&](const int index) { return Slicer<dynamic, 0, 1, 2>::slice(textureMapping, index); };

    for (auto y = 0; y < screen.height(); ++y)
    {
        for (auto x = 0; x < screen.width(); ++x)
        {
            const auto point = Vector3{{static_cast<double>(x), static_cast<double>(y), 0.0}};
            const auto coord = interpolation(point);

            if (between(coord(0), 0.0, 1.0) && between(coord(1), 0.0, 1.0) && between(coord(2), 0.0, 1.0) &&
                between(coord(3), 0.0, 1.0))
            {
                const auto textureVertex =
                    getRounded(coord(0) * tex(0) + coord(1) * tex(1) + coord(2) * tex(2) + coord(3) * tex(3));

                screen(x, y) = texture(textureVertex.x(), textureVertex.y());
            }
        }
    }
}

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

    SECTION("line equation")
    {
        SECTION("non-parallel and non-vertical")
        {
            const auto a1 = Vector2{{0.0, 1.0}};
            const auto a2 = Vector2{{2.0, 4.0}};

            const auto b1 = Vector2{{4.0, 4.0}};
            const auto b2 = Vector2{{3.0, -3.0}};

            const auto a = Line{a1, a2};
            const auto b = Line{b1, b2};

            const auto expectedIntersection = Vector2{{4.54545, 7.81818}};

            auto intersection = Vector2{};

            REQUIRE(a.intersect(b, intersection));

            REQUIRE(intersection == Tolerance(expectedIntersection));
        }

        SECTION("parallel and non-vertical")
        {
            const auto a1 = Vector2{{1.0, 1.0}};
            const auto a2 = Vector2{{2.0, 4.0}};

            const auto b1 = Vector2{{3.0, 2.0}};
            const auto b2 = Vector2{{4.0, 5.0}};

            const auto a = Line{a1, a2};
            const auto b = Line{b1, b2};

            auto intersection = Vector2{};

            REQUIRE(!a.intersect(b, intersection));
        }

        SECTION("non-parallel and vertical")
        {
            const auto a1 = Vector2{{1.0, 1.0}};
            const auto a2 = Vector2{{1.0, 4.0}};

            const auto b1 = Vector2{{2.0, 5.0}};
            const auto b2 = Vector2{{4.0, 5.0}};

            const auto a = Line{a1, a2};
            const auto b = Line{b1, b2};

            const auto expectedIntersection = Vector2{{1.0, 5.0}};

            auto intersection = Vector2{};

            REQUIRE(a.intersect(b, intersection));

            REQUIRE(intersection == Tolerance(expectedIntersection));
        }

        SECTION("parallel and vertical")
        {
            const auto a1 = Vector2{{1.0, 1.0}};
            const auto a2 = Vector2{{1.0, 2.0}};

            const auto b1 = Vector2{{3.0, 3.0}};
            const auto b2 = Vector2{{3.0, 5.0}};

            const auto a = Line{a1, a2};
            const auto b = Line{b1, b2};

            auto intersection = Vector2{};

            REQUIRE(!a.intersect(b, intersection));
        }
    }

    SECTION("convexity")
    {
        SECTION("convex")
        {
            const auto a = Vector2{{1, 3}};
            const auto b = Vector2{{3, 0}};
            const auto c = Vector2{{0, -1}};
            const auto d = Vector2{{-1, 2}};

            REQUIRE(isConvexPolygon(a, b, c, d));
        }

        SECTION("concave #1")
        {
            const auto a = Vector2{{1, 3}};
            const auto b = Vector2{{3, 0}};
            const auto c = Vector2{{0, -1}};
            const auto d = Vector2{{1, 1}};

            REQUIRE(!isConvexPolygon(a, b, c, d));
        }

        SECTION("concave #2")
        {
            const auto a = Vector2{{1, 3}};
            const auto b = Vector2{{3, 0}};
            const auto c = Vector2{{0, 0}};
            const auto d = Vector2{{-1, 0}};

            REQUIRE(!isConvexPolygon(a, b, c, d));
        }
    }

    SECTION("bilinear interpolation")
    {
        const auto texture = readBitmapFile("resources/test/dansandu/chocolate/bilinear_interpolation_texture.bmp");
        const auto textureWidth = texture.width();
        const auto textureHeight = texture.height();

        // clang-format off
        const auto textureMapping = TextureMapping{{
            {0.0,                0.0,               },
            {0.0,                textureHeight - 1.0},
            {textureWidth - 1.0, textureHeight - 1.0},
            {textureWidth - 1.0, 0.0,               },
        }};
        // clang-format on

        const auto screenWidth = 300;
        const auto screenHeight = 200;

        auto screen = Image{screenWidth, screenHeight};

        SECTION("parallel top and bottom edges vanishing point on the top")
        {
            const auto p1 = Vector3{{80.0, 20.0, 0.0}};
            const auto p2 = Vector3{{10.0, 180.0, 0.0}};
            const auto p3 = Vector3{{280.0, 180.0, 0.0}};
            const auto p4 = Vector3{{200.0, 20.0, 0.0}};

            const auto interpolation = canInterpolateBilineary(p1, p2, p3, p4);

            REQUIRE(interpolation.has_value());

            rasterizeTexture(texture, textureMapping, interpolation.value(), screen);

            REQUIRE(checkImage(screen, "bilinear_interpolation_top_bottom_parallel_top_vanish.bmp"));
        }

        SECTION("parallel left and right edges vanishing point on right")
        {
            const auto p1 = Vector3{{50.0, 10.0, 0.0}};
            const auto p2 = Vector3{{50.0, 180.0, 0.0}};
            const auto p3 = Vector3{{240.0, 140.0, 0.0}};
            const auto p4 = Vector3{{240.0, 50.0, 0.0}};

            const auto interpolation = canInterpolateBilineary(p1, p2, p3, p4);

            REQUIRE(interpolation.has_value());

            rasterizeTexture(texture, textureMapping, interpolation.value(), screen);

            REQUIRE(checkImage(screen, "bilinear_interpolation_left_right_parallel_right_vanish.bmp"));
        }
    }
}
