#include "dansandu/chocolate/geometry/clipping.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/file_system.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/gif.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/chocolate/geometry/plane.hpp"
#include "dansandu/chocolate/raster/wireframe.hpp"
#include "dansandu/chocolate/transform.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/matrix.hpp"
#include "dansandu/range/range.hpp"

using dansandu::ballotin::file_system::readBinaryFile;
using dansandu::ballotin::file_system::writeBinaryFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::gif::getGifBinary;
using dansandu::canvas::image::Image;
using dansandu::chocolate::Triangles;
using dansandu::chocolate::Vertices;
using dansandu::chocolate::geometry::clipping::clip;
using dansandu::chocolate::geometry::clipping::cull;
using dansandu::chocolate::geometry::plane::generatePlane;
using dansandu::chocolate::raster::wireframe::drawWireframe;
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

        const auto expectedVertices = Vertices{{{3.27273f, 3.27273f, 0.0f, 3.27273f},
                                                {3.27273f, 0.00000f, 0.0f, 3.27273f},
                                                {-2.76923f, 0.00000f, 0.0f, 2.76923f},
                                                {-2.76923f, 2.76923f, 0.0f, 2.76923f}}};

        const auto triangles = Triangles{{0, 1, 2}};

        const auto expectedTriangles = Triangles{{{0, 1, 2}, {0, 2, 3}}};

        const auto [clippedVertices, clippedTriangles] = clip(vertices, triangles);

        REQUIRE(close(clippedVertices, expectedVertices, 10.0e-5f));

        REQUIRE(clippedTriangles == expectedTriangles);
    }

    SECTION("culling")
    {
        const auto triangles = Triangles{{0, 1, 2}};

        SECTION("visible")
        {
            const auto vertices =
                Vertices{{{0.0f, 12.0f, 0.0f, 3.0f}, {-12.0f, 0.0f, 0.0f, 2.0f}, {12.0f, 0.0f, 0.0f, 4.0f}}};

            const auto culledTriangles = cull(vertices, triangles);

            REQUIRE(culledTriangles == triangles);
        }

        SECTION("back-facing")
        {
            const auto vertices =
                Vertices{{{-12.0f, 0.0f, 0.0f, 2.0f}, {0.0f, 12.0f, 0.0f, 3.0f}, {12.0f, 0.0f, 0.0f, 4.0f}}};

            auto culledTriangles = cull(vertices, triangles);

            REQUIRE(culledTriangles == Triangles{});
        }
    }

    SECTION("clip and cull")
    {
        auto frames = std::vector<Image>{};

        const auto [vertices, triangles] = generatePlane(2000.0f, 2000.0f, 50, 50);
        const auto width = 200;
        const auto height = 200;
        const auto frameCount = 400;

        for (auto i = 0; i < frameCount; ++i)
        {
            const auto radians = i * 1.0f * pi<float> / (frameCount - 1);
            const auto transform = rotateByX(0.5f * pi<float>) * rotateByY(radians) * translate(0.0, 100.0, -500.0) *
                                   perspective(1.0, 2000.0, 1.92, 1.0);

            auto mesh = clip(vertices * transform, triangles);
            mesh.first = dehomogenized(mesh.first) * viewport(width - 1, height - 1);
            mesh.second = cull(mesh.first, mesh.second);

            auto frame = Image{width, height};
            drawWireframe(mesh.first, mesh.second, Colors::turquoise, frame);
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
