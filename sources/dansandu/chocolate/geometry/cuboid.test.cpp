#include "dansandu/chocolate/geometry/cuboid.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/file_system.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/gif.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/raster/drawing.hpp"
#include "dansandu/chocolate/transformation.hpp"
#include "dansandu/math/common.hpp"

using dansandu::ballotin::file_system::readBinaryFile;
using dansandu::ballotin::file_system::writeBinaryFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::gif::getGifBinary;
using dansandu::canvas::image::Image;
using dansandu::chocolate::transposed;
using dansandu::chocolate::geometry::cuboid::generateCuboid;
using dansandu::chocolate::raster::drawing::drawWireframe;
using dansandu::math::common::pi;

using namespace dansandu::chocolate::transformation;

TEST_CASE("cuboid")
{
    auto images = std::vector<Image>{};

    const auto [vertices, triangles] = generateCuboid(150.0, 150.0, 150.0);
    const auto width = 200;
    const auto height = 200;
    const auto frameCount = 100;

    for (auto i = 0; i < frameCount; ++i)
    {
        const auto radians = i * 1.0f * pi<float> / (frameCount - 1);
        const auto transformation =
            transposed(perspective(1.0, 2000.0, 1.92, 1.0) * translate(0.0, 0.0, -200.0) * rotateByY(radians));

        auto tVertices = vertices * transformation;
        tVertices = dehomogenized(tVertices) * transposed(viewport(width, height));

        auto frame = Image{width, height};
        drawWireframe(tVertices, triangles, Colors::magenta, frame);
        images.push_back(std::move(frame));
    }

    auto frames = std::vector<const Image*>{};
    for (const auto& image : images)
    {
        frames.push_back(&image);
    }

    const auto delayCentiseconds = 3;
    const auto actual = getGifBinary(frames, delayCentiseconds);

    auto cuboidAnimationMatchesGif = (actual == readBinaryFile("resources/dansandu/chocolate/expected_cuboid.gif"));
    if (!cuboidAnimationMatchesGif)
    {
        writeBinaryFile("target/actual_cuboid.gif", actual);
    }
    REQUIRE(cuboidAnimationMatchesGif);
}
