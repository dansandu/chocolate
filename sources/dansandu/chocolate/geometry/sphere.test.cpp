#include "dansandu/chocolate/geometry/sphere.hpp"
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
using dansandu::chocolate::geometry::sphere::generateSphere;
using dansandu::chocolate::raster::drawing::drawWireframe;
using dansandu::math::common::pi;

using namespace dansandu::chocolate::transformation;

TEST_CASE("sphere")
{
    auto images = std::vector<Image>{};

    const auto [vertices, triangles] = generateSphere(150.0f, 16, 16);
    const auto width = 200;
    const auto height = 200;
    const auto frameCount = 100;

    for (auto i = 0; i < frameCount; ++i)
    {
        const auto radians = i * 0.125f * pi<float> / (frameCount - 1);
        const auto transformation =
            rotateByY(radians) * translate(0.0, 0.0, -200.0) * perspective(1.0, 2000.0, 1.92, 1.0);

        auto tVertices = vertices * transformation;
        tVertices = dehomogenized(tVertices) * viewport(width, height);

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

    auto sphereMatchesGif = (actual == readBinaryFile("resources/dansandu/chocolate/expected_sphere.gif"));
    if (!sphereMatchesGif)
    {
        writeBinaryFile("target/actual_sphere.gif", actual);
    }
    REQUIRE(sphereMatchesGif);
}
