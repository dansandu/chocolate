#include "dansandu/chocolate/raster/drawing.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/file_system.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/gif.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/geometry/clipping.hpp"
#include "dansandu/chocolate/geometry/sphere.hpp"
#include "dansandu/chocolate/transform.hpp"
#include "dansandu/math/common.hpp"

using dansandu::ballotin::file_system::readBinaryFile;
using dansandu::ballotin::file_system::writeBinaryFile;
using dansandu::ballotin::string::format;
using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::gif::getGifBinary;
using dansandu::canvas::image::Image;
using dansandu::chocolate::Normals;
using dansandu::chocolate::Triangles;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::Vector3Slicer;
using dansandu::chocolate::Vertices;
using dansandu::chocolate::geometry::clipping::clip;
using dansandu::chocolate::geometry::clipping::cull;
using dansandu::chocolate::geometry::sphere::generateSphere;
using dansandu::chocolate::raster::drawing::drawFlat;
using dansandu::chocolate::raster::drawing::drawWireframe;
using dansandu::chocolate::transform::dehomogenized;
using dansandu::chocolate::transform::perspective;
using dansandu::chocolate::transform::rotateByY;
using dansandu::chocolate::transform::translate;
using dansandu::chocolate::transform::viewport;
using dansandu::math::common::pi;
using dansandu::math::matrix::normalized;

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

TEST_CASE("drawing")
{
    SECTION("flat")
    {
        auto images = std::vector<Image>{};

        const auto [vertices, triangles] = generateSphere(150.0f, 18, 18);

        const auto width = 200;
        const auto height = 200;
        const auto frameCount = 100;

        const auto diffuseDirection = normalized(Vector3{{1.0, -1.0, -1.0}});

        for (auto i = 0; i < frameCount; ++i)
        {
            const auto rotation = i * 0.22f * pi<float> / (frameCount - 1);
            const auto transform = rotateByY(rotation) * translate(0.0, 0.0, -250.0);

            auto mesh = std::make_tuple(vertices * transform, triangles, Normals{});

            auto [culledTriangles, normals] = cull(std::get<0>(mesh), triangles);

            std::get<1>(mesh) = std::move(culledTriangles);

            std::get<2>(mesh) = std::move(normals);

            mesh = clip(std::get<0>(mesh) * perspective(1.0, 2000.0, 1.92, 1.0), std::get<1>(mesh), std::get<2>(mesh));

            std::get<0>(mesh) = dehomogenized(std::get<0>(mesh)) * viewport(width - 1, height - 1);

            auto frame = Image{width, height};
            drawFlat(std::get<0>(mesh), std::get<1>(mesh), std::get<2>(mesh), Colors::black, Colors::magenta,
                     diffuseDirection, frame);
            images.push_back(std::move(frame));
        }

        auto frames = std::vector<const Image*>{};
        for (const auto& image : images)
        {
            frames.push_back(&image);
        }

        const auto delayCentiseconds = 3;
        const auto actual = getGifBinary(frames, delayCentiseconds);

        auto match = actual == readBinaryFile("resources/dansandu/chocolate/expected_flat_shading.gif");
        if (!match)
        {
            writeBinaryFile("target/actual_flat_shading.gif", actual);
        }
        REQUIRE(match);
    }

    SECTION("wireframe")
    {
        const auto vertices =
            Vertices{{{49.0f, 0.0f, 35.0f, 1.0f}, {0.0f, 10.0f, 0.85f, 1.0f}, {20.0f, 49.0f, 0.0f, 1.0f}}};
        const auto triangles = Triangles{{0, 1, 2}};

        auto actual = Image{50, 50};

        drawWireframe(vertices, triangles, Colors::green, actual);

        REQUIRE_IMAGE(actual, "wireframe.bmp");
    }
}
