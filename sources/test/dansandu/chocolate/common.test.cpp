#include "dansandu/chocolate/common.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::canvas::color::Color;
using dansandu::chocolate::toColor;
using dansandu::chocolate::Vector3;

TEST_CASE("common")
{
    SECTION("toColor")
    {
        SECTION("outside [0, 1] interval")
        {
            const auto vector = Vector3{{-5.0, -0.1, 2.0}};

            const auto color = toColor(vector);

            REQUIRE(color.getRedChannel() == 0);

            REQUIRE(color.getGreenChannel() == 0);

            REQUIRE(color.getBlueChannel() == 255);
        }

        SECTION("within [0, 1] interval")
        {
            const auto vector = Vector3{{0.0, 0.2, 1.0}};

            const auto color = toColor(vector);

            REQUIRE(color.getRedChannel() == 0);

            REQUIRE(color.getGreenChannel() == 51);

            REQUIRE(color.getBlueChannel() == 255);
        }
    }
}
