#include "dansandu/chocolate/geometry/sphere.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/file_system.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/gif.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/raster/wireframe.hpp"
#include "dansandu/chocolate/transform.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/range/range.hpp"

using dansandu::ballotin::file_system::readBinaryFile;
using dansandu::ballotin::file_system::writeBinaryFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::gif::getGifBinary;
using dansandu::canvas::image::Image;
using dansandu::chocolate::geometry::sphere::generateSphere;
using dansandu::chocolate::raster::wireframe::drawWireframe;
using dansandu::chocolate::transform::dehomogenized;
using dansandu::chocolate::transform::perspective;
using dansandu::chocolate::transform::rotateByY;
using dansandu::chocolate::transform::translate;
using dansandu::chocolate::transform::viewport;
using dansandu::math::common::pi;

using namespace dansandu::range::range;

TEST_CASE("sphere")
{
    auto frames = std::vector<Image>{};

    const auto [vertices, triangles] = generateSphere(150.0f, 16, 16);
    const auto width = 200;
    const auto height = 200;
    const auto frameCount = 100;

    for (auto i = 0; i < frameCount; ++i)
    {
        const auto radians = i * 0.125f * pi<float> / (frameCount - 1);
        const auto tVertices = dehomogenized(vertices * rotateByY(radians) * translate(0.0, 0.0, -200.0) *
                                             perspective(1.0, 2000.0, 1.92, 1.0)) *
                               viewport(width - 1, height - 1);

        auto frame = Image{width, height};
        drawWireframe(tVertices, triangles, Colors::magenta, frame);
        frames.push_back(std::move(frame));
    }

    const auto delayCentiseconds = 3;
    const auto actual = getGifBinary(frames | map([](auto& f) { return &f; }) | toVector(), delayCentiseconds);

    auto match = actual == readBinaryFile("resources/dansandu/chocolate/expected_sphere.gif");
    if (!match)
    {
        writeBinaryFile("target/actual_sphere.gif", actual);
    }
    REQUIRE(match);
}
