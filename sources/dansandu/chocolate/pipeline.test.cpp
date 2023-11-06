#include "dansandu/chocolate/pipeline.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/file_system.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/gif.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/geometry/plane.hpp"
#include "dansandu/chocolate/transformation.hpp"
#include "dansandu/math/common.hpp"

using dansandu::ballotin::file_system::readBinaryFile;
using dansandu::ballotin::file_system::writeBinaryFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::gif::getGifBinary;
using dansandu::canvas::image::Image;
using dansandu::chocolate::geometry::plane::generatePlane;
using dansandu::chocolate::pipeline::Pipeline;
using dansandu::math::common::pi;

using namespace dansandu::chocolate::transformation;

TEST_CASE("pipeline")
{
    SECTION("clipping")
    {
        const auto pipeline = Pipeline{}.clipping(true);

        const auto [vertices, triangles] = generatePlane(1000.0f, 1000.0f, 36, 36);

        const auto width = 200;
        const auto height = 200;
        const auto frameCount = 400;

        auto images = std::vector<Image>{};
        for (auto i = 0; i < frameCount; ++i)
        {
            const auto rotation = i * 1.0f * pi<float> / (frameCount - 1);
            const auto transformation =
                translate(0.0f, -20.0f, -100.0f) * rotateByY(rotation) * rotateByX(0.5f * pi<float>);

            auto image = Image{width, height};
            pipeline.renderWireframe(vertices, triangles, transformation, Colors::magenta, image);
            images.push_back(std::move(image));
        }

        auto frames = std::vector<const Image*>{};
        for (const auto& image : images)
        {
            frames.push_back(&image);
        }

        const auto delayCentiseconds = 3;
        const auto actual = getGifBinary(frames, delayCentiseconds);

        auto pipelineClipMatchesGif =
            (actual == readBinaryFile("resources/dansandu/chocolate/expected_pipeline_clip.gif"));
        if (!pipelineClipMatchesGif)
        {
            writeBinaryFile("target/actual_pipeline_clip.gif", actual);
        }
        REQUIRE(pipelineClipMatchesGif);
    }
}
