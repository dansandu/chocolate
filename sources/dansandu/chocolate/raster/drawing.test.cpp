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
#include "dansandu/chocolate/geometry/plane.hpp"
#include "dansandu/chocolate/geometry/sphere.hpp"
#include "dansandu/chocolate/transformation.hpp"
#include "dansandu/math/common.hpp"

using dansandu::ballotin::file_system::readBinaryFile;
using dansandu::ballotin::file_system::writeBinaryFile;
using dansandu::ballotin::string::format;
using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::gif::getGifBinary;
using dansandu::canvas::image::Image;
using dansandu::chocolate::dynamic;
using dansandu::chocolate::Normals;
using dansandu::chocolate::TextureMapping;
using dansandu::chocolate::Triangles;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::Vertices;
using dansandu::chocolate::geometry::clipping::clip;
using dansandu::chocolate::geometry::clipping::cull;
using dansandu::chocolate::geometry::cuboid::generateCuboid;
using dansandu::chocolate::geometry::plane::generatePlane;
using dansandu::chocolate::geometry::sphere::generateSphere;
using dansandu::chocolate::raster::drawing::drawFlat;
using dansandu::chocolate::raster::drawing::drawTexture;
using dansandu::chocolate::raster::drawing::drawWireframe;
using dansandu::math::common::pi;
using dansandu::math::matrix::normalized;
using dansandu::math::matrix::Slicer;

using namespace dansandu::chocolate::transformation;

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

            std::get<0>(mesh) = dehomogenized(std::get<0>(mesh)) * viewport(width, height);

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

        const auto [vertices, triangles] = generatePlane(100.0f, 100.0f, 5, 5);

        const auto textureCoodinates = vertices * translate(50.0f, 50.0f, 0.0f);

        const auto textureMapping =
            TextureMapping{Slicer<0, 0, dynamic, 2>::slice(textureCoodinates, textureCoodinates.rowCount())};

        const auto tVertices = vertices * shearX(0.5, 0.0) * scale(1.0, -1.0, 1.0) * translate(100.0f, 100.0f, 10.0f);

        auto image = Image{200, 200, Colors::davysGrey};
        drawTexture(tVertices, triangles, textureMapping, texture, image);

        REQUIRE_IMAGE(image, "simple_texture.bmp");
    }

    SECTION("perspective texture")
    {
        const auto width = 200;
        const auto height = 200;

        const auto texture = readBitmapFile("resources/dansandu/chocolate/simple_texture.bmp");

        const auto [vertices, triangles] = generatePlane(100.0f, 100.0f, 5, 5);

        const auto textureCoodinates = vertices * translate(50.0f, 50.0f, 0.0f);

        const auto textureMapping =
            TextureMapping{Slicer<0, 0, dynamic, 2>::slice(textureCoodinates, textureCoodinates.rowCount())};

        const auto rotation = -0.25f * pi<float>;

        const auto transform = rotateByX(rotation) * translate(0.0, 0.0, -80.0) * perspective(1.0, 2000.0, 1.92, 1.0);

        const auto tVertices = dehomogenized(vertices * transform) * viewport(width, height);

        auto image = Image{width, height, Colors::davysGrey};

        drawTexture(tVertices, triangles, textureMapping, texture, image);

        REQUIRE_IMAGE(image, "perspective_texture.bmp");
    }
}
