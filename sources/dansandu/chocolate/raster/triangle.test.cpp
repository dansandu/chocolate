#include "dansandu/chocolate/raster/triangle.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/common.hpp"

#include <string>

using dansandu::ballotin::string::format;
using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::image::Image;
using dansandu::chocolate::ConstantVector3View;
using dansandu::chocolate::toColor;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::raster::triangle::drawTriangle;

static void REQUIRE_IMAGE(const Image& actualImage, const std::string& fileName)
{
    const auto expectedImagePath = "resources/dansandu/chocolate/expected_" + fileName;
    const auto expectedImage = readBitmapFile(expectedImagePath);
    if (actualImage == expectedImage)
    {
        SUCCEED("images match");
    }
    else
    {
        const auto actualImagePath = "target/actual_" + fileName;
        writeBitmapFile(actualImagePath, actualImage);
        FAIL(format("actual image does not match expected image ", expectedImagePath, " -- check ", actualImagePath,
                    " for comparison"));
    }
}

TEST_CASE("triangle")
{
    SECTION("tracing")
    {
        auto image = Image{50, 50};

        const auto solidShader = [&](const ConstantVector3View vertex, const float u, const float v, const float w)
        {
            const auto red = Vector3{{1.0, 0.0, 0.0}};
            const auto green = Vector3{{0.0, 1.0, 0.0}};
            const auto blue = Vector3{{0.0, 0.0, 1.0}};
            const auto color = u * red + v * green + w * blue;
            image(vertex.x(), vertex.y()) = toColor(color);
        };

        const auto wireframeShader = [&](const ConstantVector3View vertex, const float, const float, const float)
        {
            auto& color = image(vertex.x(), vertex.y());
            color = color == Colors::black ? Colors::red : Colors::white;
        };

        SECTION("flat top triangle with bottom vertex to the left")
        {
            const auto a = Vector3{{15.0, 0.0, 0.0}};
            const auto b = Vector3{{49.0, 0.0, 0.0}};
            const auto c = Vector3{{0.0, 49.0, 0.0}};

            drawTriangle(a, b, c, true, solidShader);

            drawTriangle(a, b, c, false, wireframeShader);

            REQUIRE_IMAGE(image, "flat_top_triangle_bottom_to_left.bmp");
        }

        SECTION("flat top triangle with bottom vertex to the right")
        {
            const auto a = Vector3{{15.0, 0.0, 0.0}};
            const auto b = Vector3{{0.0, 0.0, 0.0}};
            const auto c = Vector3{{49.0, 49.0, 0.0}};

            drawTriangle(a, b, c, true, solidShader);

            drawTriangle(a, b, c, false, wireframeShader);

            REQUIRE_IMAGE(image, "flat_top_triangle_bottom_to_right.bmp");
        }

        SECTION("flat bottom tip to the left")
        {
            const auto a = Vector3{{49.0, 49.0, 0.0}};
            const auto b = Vector3{{25.0, 49.0, 0.0}};
            const auto c = Vector3{{0.0, 0.0, 0.0}};

            drawTriangle(a, b, c, true, solidShader);

            drawTriangle(a, b, c, false, wireframeShader);

            REQUIRE_IMAGE(image, "flat_bottom_triangle_tip_to_the_left.bmp");
        }

        SECTION("flat bottom tip to the right")
        {
            const auto a = Vector3{{25.0, 49.0, 0.0}};
            const auto b = Vector3{{0.0, 49.0, 0.0}};
            const auto c = Vector3{{49.0, 0.0, 0.0}};

            drawTriangle(a, b, c, true, solidShader);

            drawTriangle(a, b, c, false, wireframeShader);

            REQUIRE_IMAGE(image, "flat_bottom_triangle_tip_to_the_right.bmp");
        }

        SECTION("flat bottom tip to the left sharp")
        {
            const auto a = Vector3{{0.0, 0.0, 0.0}};
            const auto b = Vector3{{30.0, 20.0, 0.0}};
            const auto c = Vector3{{49.0, 20.0, 0.0}};

            drawTriangle(a, b, c, true, solidShader);

            drawTriangle(a, b, c, false, wireframeShader);

            REQUIRE_IMAGE(image, "flat_bottom_triangle_tip_to_the_left_sharp.bmp");
        }

        SECTION("obtuse triangle")
        {
            const auto a = Vector3{{49.0, 35.0, 0.0}};
            const auto b = Vector3{{0.0, 0.0, 0.0}};
            const auto c = Vector3{{5.0, 19.0, 0.0}};

            drawTriangle(a, b, c, true, solidShader);

            drawTriangle(a, b, c, false, wireframeShader);

            REQUIRE_IMAGE(image, "obtuse_triangle.bmp");
        }
    }

    SECTION("barycentric coordinates")
    {
        SECTION("skewed")
        {
            const auto a = Vector3{{10.0, 15.0, 0.0}};
            const auto b = Vector3{{15.0, 4.0, 0.0}};
            const auto c = Vector3{{13.0, 10.0, 0.0}};

            const auto shader = [&](const auto vertex, const auto u, const auto v, const auto w)
            {
                const auto d = u * a + v * b + w * c;
                if (!close(vertex, d, 1.0e-6f))
                {
                    FAIL(format("vertex ", vertex, "does not match point ", d, " with u=", u, " v=", v, " w=", w));
                }
                else
                {
                    SUCCEED("barycentric coordinates hold");
                }
            };

            drawTriangle(a, b, c, true, shader);
        }

        /*SECTION("")
        {
            const auto a = Vector3{{10.0, 10.0, 0.0}};
            const auto b = Vector3{{10.0, 190.0, 0.0}};
            const auto c = Vector3{{190.0, 80.0, 0.0, 1.0}};

            auto image = Image{200, 200};
            drawTexture(vertices, triangles, textureMapping, texture, image);
            drawWireframe(vertices, triangles, Colors::white, image);
            writeBitmapFile("target/actual_simple_texture.bmp", image);
        }*/
    }
}
