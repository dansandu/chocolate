#include "dansandu/chocolate/raster/drawing.hpp"
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
#include "dansandu/chocolate/utility.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::ballotin::file_system::readBinaryFile;
using dansandu::ballotin::file_system::writeBinaryFile;
using dansandu::ballotin::string::format;
using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Color;
using dansandu::canvas::gif::getGifBinary;
using dansandu::canvas::image::Image;
using dansandu::chocolate::checkImage;
using dansandu::chocolate::ColumnVector;
using dansandu::chocolate::dynamic;
using dansandu::chocolate::Normals;
using dansandu::chocolate::TextureMapping;
using dansandu::chocolate::transposed;
using dansandu::chocolate::Triangles;
using dansandu::chocolate::Vector3;
using dansandu::chocolate::Vertices;
using dansandu::chocolate::geometry::clipping::clip;
using dansandu::chocolate::geometry::clipping::cull;
using dansandu::chocolate::geometry::cuboid::generatePolygonMeshCuboid;
using dansandu::chocolate::geometry::cuboid::generateTriangleMeshCuboid;
using dansandu::chocolate::geometry::plane::generatePlane;
using dansandu::chocolate::geometry::sphere::generateSphere;
using dansandu::chocolate::raster::drawing::drawFlat;
using dansandu::chocolate::raster::drawing::drawTexture;
using dansandu::chocolate::raster::drawing::drawWireframe;
using dansandu::math::pi;
using dansandu::math::matrix::normalized;
using dansandu::math::matrix::Slicer;

using namespace dansandu::chocolate::transformation;

TEST_CASE("drawing")
{
    SECTION("flat shading")
    {
        auto images = std::vector<Image>{};

        const auto [vertices, triangles] = generateSphere(150.0, 18, 18);

        const auto width = 200;
        const auto height = 200;
        const auto frameCount = 100;

        const auto diffuseDirection = normalized(Vector3{{1.0, -1.0, -1.0}});

        for (auto i = 0; i < frameCount; ++i)
        {
            const auto rotation = i * 0.22 * pi<double> / (frameCount - 1);
            const auto transform = transposed(translate(0.0, 0.0, -250.0) * rotateByY(rotation));

            auto mesh = std::make_tuple(vertices * transform, triangles, Normals{});

            auto [culledTriangles, normals] = cull(std::get<0>(mesh), triangles);

            std::get<0>(mesh) = std::get<0>(mesh) * transposed(perspective(1.0, 2000.0, 1.92, 1.0));

            std::get<1>(mesh) = std::move(culledTriangles);

            std::get<2>(mesh) = std::move(normals);

            mesh = clip(std::get<0>(mesh), std::get<1>(mesh), std::get<2>(mesh));

            std::get<0>(mesh) = dehomogenized(std::get<0>(mesh)) * transposed(viewport(width, height));

            auto frame = Image{width, height};
            drawFlat(std::get<0>(mesh), std::get<1>(mesh), std::get<2>(mesh), Color::black, Color::magenta,
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
            (actual == readBinaryFile("resources/test/dansandu/chocolate/expected_flat_shading.gif"));
        if (!flatShadingMatchesGif)
        {
            writeBinaryFile("target/temporary/actual_flat_shading.gif", actual);
        }
        REQUIRE(flatShadingMatchesGif);
    }

    SECTION("wireframe shading")
    {
        const auto vertices = Vertices{{{49.0, 0.0, 35.0, 1.0}, {0.0, 10.0, 0.85, 1.0}, {20.0, 49.0, 0.0, 1.0}}};
        const auto triangles = Triangles{{0, 1, 2}};

        auto actual = Image{50, 50};

        drawWireframe(vertices, triangles, Color::green, actual);

        REQUIRE(checkImage(actual, "wireframe.bmp"));
    }

    SECTION("triangle texture")
    {
        const auto width = 200;
        const auto height = 200;

        const auto texture = readBitmapFile("resources/test/dansandu/chocolate/simple_texture.bmp");

        const auto [vertices, triangles] = generatePlane(100.0, 100.0, 2, 2);

        const auto textureCoodinates = vertices * transposed(translate(50.0, 50.0, 0.0));

        const auto textureMapping =
            TextureMapping{Slicer<0, 0, dynamic, 2>::slice(textureCoodinates, textureCoodinates.rowCount())};

        const auto rotation = -0.25 * pi<double>;

        const auto transform =
            transposed(perspective(1.0, 2000.0, 1.92, 1.0) * translate(0.0, 0.0, -80.0) * rotateByX(rotation));

        auto tVertices = vertices * transform;

        const auto depth = ColumnVector{Slicer<0, 3, dynamic, 1>::slice(tVertices, tVertices.rowCount())};

        tVertices = dehomogenized(tVertices) * transposed(viewport(width, height));

        auto image = Image{width, height};

        drawTexture(tVertices, depth, triangles, textureMapping, texture, image);

        REQUIRE(checkImage(image, "perspective_texture.bmp"));
    }

    SECTION("triangle mesh cuboid texture")
    {
        auto images = std::vector<Image>{};

        const auto [vertices, triangles] = generateTriangleMeshCuboid(150.0, 150.0, 150.0);
        const auto width = 200;
        const auto height = 200;
        const auto frameCount = 200;

        const auto texture = readBitmapFile("resources/test/dansandu/chocolate/cuboid_texture.bmp");

        // clang-format off
        const auto textureMapping = TextureMapping{{
            {  0.0,   0.0},
            {  0.0,  50.0},
            { 50.0,  50.0},
            { 50.0,   0.0},

            { 50.0,   0.0},
            { 50.0,  50.0},
            {100.0,  50.0},
            {100.0,   0.0},

            {100.0,   0.0},
            {100.0,  50.0},
            {150.0,  50.0},
            {150.0,   0.0},

            {  0.0,  50.0},
            {  0.0, 100.0},
            { 50.0, 100.0},
            { 50.0,  50.0},

            { 50.0,  50.0},
            { 50.0, 100.0},
            {100.0, 100.0},
            {100.0,  50.0},

            {100.0,  50.0},
            {100.0, 100.0},
            {150.0, 100.0},
            {150.0,  50.0},
        }};
        // clang-format on

        for (auto i = 0; i < frameCount; ++i)
        {
            const auto radians = i * 2.0 * pi<double> / (frameCount - 1);

            const auto transformation =
                transposed(translate(0.0, -50.0, -220.0) * rotateByY(radians) * rotateByX(radians));

            auto tVertices = vertices * transformation;
            auto [culledTriangles, normals] = cull(tVertices, triangles);

            tVertices = tVertices * transposed(perspective(1.0, 2000.0, 1.92, 1.0));

            const auto depth = ColumnVector{Slicer<0, 3, dynamic, 1>::slice(tVertices, tVertices.rowCount())};

            tVertices = dehomogenized(tVertices) * transposed(viewport(width, height));

            auto frame = Image{width, height};

            drawTexture(tVertices, depth, culledTriangles, textureMapping, texture, frame);
            images.push_back(std::move(frame));
        }

        auto frames = std::vector<const Image*>{};
        for (const auto& image : images)
        {
            frames.push_back(&image);
        }

        const auto delayCentiseconds = 3;
        const auto actual = getGifBinary(frames, delayCentiseconds);

        auto cuboidAnimationMatchesGif =
            (actual == readBinaryFile("resources/test/dansandu/chocolate/expected_triangle_mesh_cuboid_texture.gif"));
        if (!cuboidAnimationMatchesGif)
        {
            writeBinaryFile("target/temporary/actual_triangle_mesh_cuboid_texture.gif", actual);
        }
        REQUIRE(cuboidAnimationMatchesGif);
    }

    SECTION("polygon texture")
    {
        auto images = std::vector<Image>{};

        const auto [vertices, polygons] = generatePolygonMeshCuboid(150.0, 150.0, 150.0);
        const auto width = 200;
        const auto height = 200;
        const auto frameCount = 200;

        const auto texture = readBitmapFile("resources/test/dansandu/chocolate/cuboid_texture.bmp");

        // clang-format off
        const auto textureMapping = TextureMapping{{
            {  0.0,   0.0},
            {  0.0,  50.0},
            { 50.0,  50.0},
            { 50.0,   0.0},

            { 50.0,   0.0},
            { 50.0,  50.0},
            {100.0,  50.0},
            {100.0,   0.0},

            {100.0,   0.0},
            {100.0,  50.0},
            {150.0,  50.0},
            {150.0,   0.0},

            {  0.0,  50.0},
            {  0.0, 100.0},
            { 50.0, 100.0},
            { 50.0,  50.0},

            { 50.0,  50.0},
            { 50.0, 100.0},
            {100.0, 100.0},
            {100.0,  50.0},

            {100.0,  50.0},
            {100.0, 100.0},
            {150.0, 100.0},
            {150.0,  50.0},
        }};
        // clang-format on

        for (auto i = 0; i < frameCount; ++i)
        {
            const auto radians = i * 2.0 * pi<double> / (frameCount - 1);

            const auto transformation =
                transposed(translate(0.0, -50.0, -220.0) * rotateByY(radians) * rotateByX(radians));

            auto tVertices = vertices * transformation;
            auto [culledPolygons, normals] = cull(tVertices, polygons);

            tVertices = tVertices * transposed(perspective(1.0, 2000.0, 1.92, 1.0));
            tVertices = dehomogenized(tVertices) * transposed(viewport(width, height));

            auto frame = Image{width, height};

            drawTexture(tVertices, culledPolygons, textureMapping, texture, frame);
            images.push_back(std::move(frame));
        }

        auto frames = std::vector<const Image*>{};
        for (const auto& image : images)
        {
            frames.push_back(&image);
        }

        const auto delayCentiseconds = 3;
        const auto actual = getGifBinary(frames, delayCentiseconds);

        auto cuboidAnimationMatchesGif =
            (actual == readBinaryFile("resources/test/dansandu/chocolate/expected_polygon_texture.gif"));
        if (!cuboidAnimationMatchesGif)
        {
            writeBinaryFile("target/temporary/actual_polygon_texture.gif", actual);
        }
        REQUIRE(cuboidAnimationMatchesGif);
    }
}
