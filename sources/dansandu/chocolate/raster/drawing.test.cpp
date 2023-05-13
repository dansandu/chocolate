#include "dansandu/chocolate/raster/drawing.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/file_system.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/gif.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/geometry/clipping.hpp"
#include "dansandu/chocolate/geometry/cuboid.hpp"
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
using dansandu::chocolate::TextureMapping;
using dansandu::chocolate::Triangles;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::Vector3Slicer;
using dansandu::chocolate::Vertices;
using dansandu::chocolate::geometry::clipping::clip;
using dansandu::chocolate::geometry::clipping::cull;
using dansandu::chocolate::geometry::cuboid::generateCuboid;
using dansandu::chocolate::geometry::sphere::generateSphere;
using dansandu::chocolate::raster::drawing::drawFlat;
using dansandu::chocolate::raster::drawing::drawTexture;
using dansandu::chocolate::raster::drawing::drawWireframe;
using dansandu::chocolate::transform::dehomogenized;
using dansandu::chocolate::transform::perspective;
using dansandu::chocolate::transform::rotateByY;
using dansandu::chocolate::transform::rotateByZ;
using dansandu::chocolate::transform::translate;
using dansandu::chocolate::transform::viewport;
using dansandu::math::common::pi;
using dansandu::math::matrix::normalized;

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

        auto flatShadingMatchesGif =
            (actual == readBinaryFile("resources/dansandu/chocolate/expected_flat_shading.gif"));
        if (!flatShadingMatchesGif)
        {
            writeBinaryFile("target/actual_flat_shading.gif", actual);
        }
        REQUIRE(flatShadingMatchesGif);
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

    SECTION("simple texture")
    {
        const auto texture = readBitmapFile("resources/dansandu/chocolate/simple_texture.bmp");

        const auto vertices = Vertices{{
            {40.0, 10.0, -3.0, 1.0},
            {10.0, 190.0, -1.0, 1.0},
            {190.0, 190.0, -1.0, 1.0},
            {160.0, 10.0, -3.0, 1.0},
        }};

        const auto triangles = Triangles{{
            {0, 1, 2},
            {0, 2, 3},
        }};

        const auto textureMapping = TextureMapping{{
            {0.0, 0.0},
            {0.0, 100.0},
            {100.0, 100.0},
            {100.0, 0.0},
        }};

        auto image = Image{200, 200};
        drawTexture(vertices, triangles, textureMapping, texture, image);
        drawWireframe(vertices, triangles, Colors::magenta, image);
        writeBitmapFile("target/actual_simple_texture.bmp", image);
    }

    /*SECTION("texture mapping")
    {
        const auto texture = readBitmapFile("resources/dansandu/chocolate/cuboid_texture.bmp");

        const auto textureMapping = TextureMapping{
            {{49.0, 50.0},  {0.0, 50.0},   {0.0, 99.0},    {49.0, 99.0},   {100.0, 99.0}, {100.0, 49.0},
             {150.0, 49.0}, {150.0, 99.0}, {50.0, 0.0},    {50.0, 49.0},   {100.0, 49.0}, {100.0, 0.0},
             {50.0, 100.0}, {50.0, 149.0}, {100.0, 149.0}, {100.0, 100.0}, {99.0, 50.0},  {50.0, 50.0},
             {50.0, 99.0},  {99.0, 99.0},  {150.0, 50.0},  {150.0, 99.0},  {199.0, 99.0}, {199.0, 50.0}}};

        auto frames = std::vector<Image>{};

        const auto [vertices, triangles] = generateCuboid(150.0, 150.0, 150.0);
        const auto width = 200;
        const auto height = 200;
        const auto frameCount = 300;

        for (auto i = 0; i < frameCount; ++i)
        {
            const auto radians = i * 2.0f * pi<float> / (frameCount - 1);

            auto tVertices = vertices * rotateByZ(0.0) * rotateByY(radians) * translate(0.0, 0.0, -200.0);

            const auto [cTriangles, normals] = cull(tVertices, triangles);

            tVertices =
                dehomogenized(tVertices * perspective(1.0, 2000.0, 1.92, 1.0)) * viewport(width - 1, height - 1);

            auto frame = Image{width, height};
            drawTexture(tVertices, cTriangles, textureMapping, texture, frame);
            frames.push_back(std::move(frame));
        }

        const auto delayCentiseconds = 3;
        const auto actual = getGifBinary(frames | map([](auto& f) { return &f; }) | toVector(), delayCentiseconds);

        auto match = actual == readBinaryFile("resources/dansandu/chocolate/expected_textured_cuboid.gif");
        if (!match)
        {
            writeBinaryFile("target/actual_textured_cuboid.gif", actual);
            FAIL("textured cuboid animation is not a match");
        }
        else
        {
            SUCCEED("textured cuboid animation is a match");
        }
    }*/
}
