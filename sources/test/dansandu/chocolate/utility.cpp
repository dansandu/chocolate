#include "dansandu/ballotin/string.hpp"
#include "dansandu/canvas/bitmap.hpp"
#include "dansandu/canvas/image.hpp"
#include "dansandu/chocolate/common.hpp"

#include <stdexcept>
#include <string>

using dansandu::ballotin::string::format;
using dansandu::canvas::bitmap::readBitmapFile;
using dansandu::canvas::bitmap::writeBitmapFile;
using dansandu::canvas::image::Image;

namespace dansandu::chocolate
{

bool checkImage(const Image& actualImage, const std::string& fileName)
{
    const auto expectedImagePath = "resources/test/dansandu/chocolate/expected_" + fileName;
    const auto expectedImage = readBitmapFile(expectedImagePath);
    if (actualImage != expectedImage)
    {
        const auto actualImagePath = "target/temporary/actual_" + fileName;
        writeBitmapFile(actualImagePath, actualImage);

        THROW(std::runtime_error, "actual image does not match expected image ", expectedImagePath, " -- check ",
              actualImagePath, " for comparison");
    }

    return true;
}

}
