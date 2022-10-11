#include "dansandu/chocolate/geometry/clipping.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/file_system.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/gif.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/chocolate/geometry/plane.hpp"
#include "dansandu/chocolate/geometry/surface.hpp"
#include "dansandu/chocolate/raster/drawing.hpp"
#include "dansandu/chocolate/transform.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/matrix.hpp"
#include "dansandu/range/range.hpp"

using dansandu::ballotin::file_system::readBinaryFile;
using dansandu::ballotin::file_system::writeBinaryFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::gif::getGifBinary;
using dansandu::canvas::image::Image;
using dansandu::chocolate::Normals;
using dansandu::chocolate::Triangles;
using dansandu::chocolate::Vertices;
using dansandu::chocolate::geometry::clipping::clip;
using dansandu::chocolate::geometry::clipping::cull;
using dansandu::chocolate::geometry::plane::generatePlane;
using dansandu::chocolate::geometry::surface::generateTriangleNormals;
using dansandu::chocolate::raster::drawing::drawWireframe;
using dansandu::chocolate::transform::dehomogenized;
using dansandu::chocolate::transform::perspective;
using dansandu::chocolate::transform::rotateByX;
using dansandu::chocolate::transform::rotateByY;
using dansandu::chocolate::transform::translate;
using dansandu::chocolate::transform::viewport;
using dansandu::math::common::pi;
using dansandu::math::matrix::close;

using namespace dansandu::range::range;

TEST_CASE("clipping")
{
    SECTION("clip")
    {
        const auto vertices =
            Vertices{{{-12.0f, 0.0f, 0.0f, 2.0f}, {0.0f, 12.0f, 0.0f, 3.0f}, {12.0f, 0.0f, 0.0f, 4.0f}}};

        const auto triangles = Triangles{{0, 1, 2}};

        const auto normals = generateTriangleNormals(vertices, triangles);

        const auto [clippedVertices, clippedTriangles, clippedNormals] = clip(vertices, triangles, normals);

        const auto expectedVertices = Vertices{{{3.27273f, 3.27273f, 0.0f, 3.27273f},
                                                {3.27273f, 0.00000f, 0.0f, 3.27273f},
                                                {-2.76923f, 0.00000f, 0.0f, 2.76923f},
                                                {-2.76923f, 2.76923f, 0.0f, 2.76923f}}};

        REQUIRE(close(clippedVertices, expectedVertices, 10.0e-5f));

        const auto expectedTriangles = Triangles{{{0, 1, 2}, {0, 2, 3}}};

        REQUIRE(clippedTriangles == expectedTriangles);

        const auto expectedNormals = Normals{{{0.0, 0.0, -1.0}, {0.0, 0.0, -1.0}}};

        REQUIRE(close(clippedNormals, expectedNormals, 10.0e-5f));
    }

    SECTION("culling")
    {
        const auto triangles = Triangles{{0, 1, 2}};

        SECTION("visible")
        {
            const auto vertices = Vertices{{{1.0, 0.0, -10.0, 1.0}, {0.0, 1.0, -10.0, 1.0}, {0.0, 0.0, -9.0, 1.0}}};

            const auto [culledTriangles, normals] = cull(vertices, triangles);

            REQUIRE(culledTriangles == triangles);

            const auto expectedNormals = Normals{{{0.57735027, 0.57735027, 0.57735027}}};

            REQUIRE(close(expectedNormals, normals, 1.0e-5f));
        }

        SECTION("back-facing")
        {
            const auto vertices =
                Vertices{{{-12.0f, 0.0f, 0.0f, 2.0f}, {0.0f, 12.0f, 0.0f, 3.0f}, {12.0f, 0.0f, 0.0f, 4.0f}}};

            const auto [culledTriangles, normals] = cull(vertices, triangles);

            REQUIRE(culledTriangles == Triangles{});
        }
    }

    SECTION("cull and clip")
    {
        auto frames = std::vector<Image>{};

        const auto [vertices, triangles] = generatePlane(4000.0f, 1000.0f, 40, 10);

        const auto width = 200;
        const auto height = 200;
        const auto frameCount = 400;

        for (auto i = 0; i < frameCount; ++i)
        {
            const auto rotation = i * 0.70f * pi<float> / (frameCount - 1);
            const auto transform = rotateByX(-rotation) * translate(0.0, 100.0, -500.0);

            auto mesh = std::make_tuple(vertices * transform, triangles, Normals{});

            auto [culledTriangles, normals] = cull(std::get<0>(mesh), triangles);
            std::get<1>(mesh) = std::move(culledTriangles);
            std::get<2>(mesh) = std::move(normals);

            mesh = clip(std::get<0>(mesh) * perspective(1.0, 2000.0, 1.92, 1.0), std::get<1>(mesh), std::get<2>(mesh));

            std::get<0>(mesh) = dehomogenized(std::get<0>(mesh)) * viewport(width - 1, height - 1);

            auto frame = Image{width, height};
            drawWireframe(std::get<0>(mesh), std::get<1>(mesh), Colors::turquoise, frame);
            frames.push_back(std::move(frame));
        }

        const auto delayCentiseconds = 3;
        const auto actual = getGifBinary(frames | map([](auto& f) { return &f; }) | toVector(), delayCentiseconds);

        auto match = actual == readBinaryFile("resources/dansandu/chocolate/expected_clip_and_cull.gif");
        if (!match)
        {
            writeBinaryFile("target/actual_clip_and_cull.gif", actual);
        }
        REQUIRE(match);
    }
}
