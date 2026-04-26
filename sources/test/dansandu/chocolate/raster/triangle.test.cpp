#include "dansandu/chocolate/raster/triangle.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/chocolate/utility.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <string>

using dansandu::ballotin::string::format;
using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Color;
using dansandu::canvas::image::Image;
using dansandu::chocolate::checkImage;
using dansandu::chocolate::ConstantVector3View;
using dansandu::chocolate::getRounded;
using dansandu::chocolate::toColor;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::raster::triangle::drawTriangle;
using dansandu::math::close;

TEST_CASE("triangle")
{
    SECTION("tracing")
    {
        auto image = Image{50, 50};

        const auto solidShader = [&](const ConstantVector3View vertex, const double u, const double v, const double w)
        {
            const auto red = Vector3{{1.0, 0.0, 0.0}};
            const auto green = Vector3{{0.0, 1.0, 0.0}};
            const auto blue = Vector3{{0.0, 0.0, 1.0}};
            const auto color = u * red + v * green + w * blue;

            const auto screen = getRounded(vertex);

            image(screen.x(), screen.y()) = toColor(color);
        };

        const auto wireframeShader = [&](const ConstantVector3View vertex, const double, const double, const double)
        {
            const auto screen = getRounded(vertex);

            auto& color = image(screen.x(), screen.y());

            color = color == Color::black ? Color::red : Color::white;
        };

        SECTION("flat top triangle with bottom vertex to the left")
        {
            const auto a = Vector3{{15.0, 0.0, 0.0}};
            const auto b = Vector3{{49.0, 0.0, 0.0}};
            const auto c = Vector3{{0.0, 49.0, 0.0}};

            drawTriangle(a, b, c, false, solidShader);

            drawTriangle(a, b, c, true, wireframeShader);

            REQUIRE(checkImage(image, "flat_top_triangle_bottom_to_left.bmp"));
        }

        SECTION("flat top triangle with bottom vertex to the right")
        {
            const auto a = Vector3{{15.0, 0.0, 0.0}};
            const auto b = Vector3{{0.0, 0.0, 0.0}};
            const auto c = Vector3{{49.0, 49.0, 0.0}};

            drawTriangle(a, b, c, false, solidShader);

            drawTriangle(a, b, c, true, wireframeShader);

            REQUIRE(checkImage(image, "flat_top_triangle_bottom_to_right.bmp"));
        }

        SECTION("flat bottom tip to the left")
        {
            const auto a = Vector3{{49.0, 49.0, 0.0}};
            const auto b = Vector3{{25.0, 49.0, 0.0}};
            const auto c = Vector3{{0.0, 0.0, 0.0}};

            drawTriangle(a, b, c, false, solidShader);

            drawTriangle(a, b, c, true, wireframeShader);

            REQUIRE(checkImage(image, "flat_bottom_triangle_tip_to_the_left.bmp"));
        }

        SECTION("flat bottom tip to the right")
        {
            const auto a = Vector3{{25.0, 49.0, 0.0}};
            const auto b = Vector3{{0.0, 49.0, 0.0}};
            const auto c = Vector3{{49.0, 0.0, 0.0}};

            drawTriangle(a, b, c, false, solidShader);

            drawTriangle(a, b, c, true, wireframeShader);

            REQUIRE(checkImage(image, "flat_bottom_triangle_tip_to_the_right.bmp"));
        }

        SECTION("flat bottom tip to the left sharp")
        {
            const auto a = Vector3{{0.0, 0.0, 0.0}};
            const auto b = Vector3{{30.0, 20.0, 0.0}};
            const auto c = Vector3{{49.0, 20.0, 0.0}};

            drawTriangle(a, b, c, false, solidShader);

            drawTriangle(a, b, c, true, wireframeShader);

            REQUIRE(checkImage(image, "flat_bottom_triangle_tip_to_the_left_sharp.bmp"));
        }

        SECTION("obtuse triangle")
        {
            const auto a = Vector3{{49.0, 35.0, 0.0}};
            const auto b = Vector3{{0.0, 0.0, 0.0}};
            const auto c = Vector3{{5.0, 19.0, 0.0}};

            drawTriangle(a, b, c, false, solidShader);

            drawTriangle(a, b, c, true, wireframeShader);

            REQUIRE(checkImage(image, "obtuse_triangle.bmp"));
        }
    }

    SECTION("barycentric coordinates")
    {
        const auto a = Vector3{{10.0, 15.0, 0.0}};
        const auto b = Vector3{{15.0, 4.0, 0.0}};
        const auto c = Vector3{{13.0, 10.0, 0.0}};

        const auto shader = [&](const auto vertex, const auto u, const auto v, const auto w)
        {
            const auto d = u * a + v * b + w * c;

            if (!close(vertex, d, 1.0e-6))
            {
                THROW(std::runtime_error, "vertex ", vertex, "does not match point ", d, " with u=", u, " v=", v,
                      " w=", w);
            }
        };

        drawTriangle(a, b, c, false, shader);
    }
}
