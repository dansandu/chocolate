#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/file_system.hpp"
#include "dansandu/canvas/color.hpp"
#include "dansandu/canvas/gif.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/math/common.hpp"

#include <cmath>

using dansandu::ballotin::file_system::writeBinaryFile;
using dansandu::canvas::color::Colors;
using dansandu::canvas::gif::getGifBinary;
using dansandu::canvas::image::Image;
using dansandu::math::common::pi;

static std::pair<double, double> model(const double x)
{
    const auto a = std::vector<double>{1.2, -1.6, -1.06, 2.0, 1.05, -0.7};
    auto value = 0.0;
    auto derivative = 0.0;
    for (auto i = 0; i < static_cast<int>(a.size()); ++i)
    {
        value += a[i] * std::pow(x, i);
        derivative += a[i] * i * std::pow(x, i - 1);
    }
    return std::make_pair(value, derivative);
}

static void drawMomentum(const double momentum)
{
    auto images = std::vector<Image>{};

    const auto width = 300;
    const auto height = 300;
    const auto frameCount = 400;

    const auto modelColor = Colors::blue;
    const auto modelSamples = 2000;

    auto background = Image{width, height, Colors::white};
    for (auto i = 0; i < modelSamples; ++i)
    {
        const auto x = i * static_cast<double>(width) / modelSamples;
        const auto y = std::round(height - height * model(2.4 * (x / width) - 1.2).first / 2.4);

        const auto pixelX = std::round(x);
        const auto pixelY = std::round(y);
        if (pixelX >= 0 && pixelX < width && pixelY >= 0 && pixelY < height)
        {
            background(pixelX, pixelY) = modelColor;
        }
    }

    const auto sphereSamples = 100;
    const auto sphereRadius = 15.0f;
    const auto sphereColor = Colors::magenta;

    const auto learningRate = 0.4;

    auto x = -20.0;
    auto update = 0.0;

    for (auto i = 0; i < frameCount; ++i)
    {
        auto image = background;

        const auto [value, derivative] = model(2.4 * (x / width) - 1.2);
        const auto y = height - height * value / 2.4;

        for (auto j = 0; j < sphereSamples; ++j)
        {
            const auto radians = j * 2.0f * pi<float> / (sphereSamples - 1);
            const auto sphereX = std::round(x + sphereRadius * std::cos(radians));
            const auto sphereY = std::round(y + sphereRadius * std::sin(radians));

            if (sphereX >= 0 && sphereX < width && sphereY >= 0 && sphereY < height)
            {
                image(sphereX, sphereY) = sphereColor;
            }
        }

        images.push_back(std::move(image));

        update = momentum * update + learningRate * derivative;

        x -= update;
    }

    auto frames = std::vector<const Image*>{};
    for (const auto& image : images)
    {
        frames.push_back(&image);
    }

    const auto delayCentiseconds = 3;
    const auto binary = getGifBinary(frames, delayCentiseconds);

    char fileNameBuffer[64];

    sprintf(fileNameBuffer, "target/momentum%.2f.gif", momentum);

    writeBinaryFile(fileNameBuffer, binary);
}

TEST_CASE("momentum")
{
    drawMomentum(0.00);

    drawMomentum(0.90);
}
