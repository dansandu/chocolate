#include "dansandu/chocolate/raster/wireframe.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"
#include "dansandu/math/common.hpp"

using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::image::Image;
using dansandu::chocolate::Point2;
using dansandu::chocolate::Triangles;
using dansandu::chocolate::Vertices;
using dansandu::chocolate::raster::wireframe::drawWireframe;

TEST_CASE("wireframe")
{
    const auto vertices =
        Vertices{{{49.0f, 0.0f, 35.0f, 1.0f}, {0.0f, 10.0f, 0.85f, 1.0f}, {20.0f, 49.0f, 0.0f, 1.0f}}};
    const auto triangles = Triangles{{0, 1, 2}};

    auto actual = Image{50, 50};

    drawWireframe(vertices, triangles, Colors::green, actual);

    const auto expected = readBitmapFile("resources/dansandu/chocolate/expected_wireframe.bmp");
    const auto success = expected == actual;
    if (!success)
    {
        writeBitmapFile("target/actual_wireframe.bmp", actual);
    }
    REQUIRE(success);
}
