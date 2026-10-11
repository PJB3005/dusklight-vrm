#include "mipmap_gen.hpp"

namespace slugcat::vrm::mipmap_gen {

namespace {

uint8_t mipmap_sample(uint8_t const* source, uint32_t sourceWidth, uint32_t sourceHeight, uint32_t mipX,
                               uint32_t mipY, uint32_t offset) {
    const auto x0 = std::min(mipX * 2, sourceWidth - 1);
    const auto x1 = std::min(x0 + 1, sourceWidth - 1);
    const auto y0 = std::min(mipY * 2, sourceHeight - 1);
    const auto y1 = std::min(y0 + 1, sourceHeight - 1);

    auto const s1 = source[(static_cast<size_t>(y0) * sourceWidth + x0) * 4 + offset];
    auto const s2 = source[(static_cast<size_t>(y1) * sourceWidth + x0) * 4 + offset];
    auto const s3 = source[(static_cast<size_t>(y0) * sourceWidth + x1) * 4 + offset];
    auto const s4 = source[(static_cast<size_t>(y1) * sourceWidth + x1) * 4 + offset];

    return (s1 + s2 + s3 + s4) / 4;
}

}

image_io::LoadedImageBuffer genMipmaps(image_io::LoadedImageBuffer buffer) {
    auto prevWidth = buffer.extent.width;
    auto prevHeight = buffer.extent.height;

    auto const* prevLayer = &buffer.buffer.at(0).at(0);

    while (true) {
        auto const mipWidth = prevWidth / 2;
        auto const mipHeight = prevHeight / 2;
        if (mipWidth == 0 || mipHeight == 0) {
            break;
        }

        std::vector<uint8_t> newLevel;
        newLevel.resize(mipWidth * mipHeight * 4);

        for (uint32_t y = 0; y < mipHeight; ++y) {
            for (uint32_t x = 0; x < mipWidth; ++x) {
                newLevel[(y * mipWidth + x) * 4 + 0] = mipmap_sample(prevLayer, prevWidth, prevHeight, x, y, 0);
                newLevel[(y * mipWidth + x) * 4 + 1] = mipmap_sample(prevLayer, prevWidth, prevHeight, x, y, 1);
                newLevel[(y * mipWidth + x) * 4 + 2] = mipmap_sample(prevLayer, prevWidth, prevHeight, x, y, 2);
                newLevel[(y * mipWidth + x) * 4 + 3] = mipmap_sample(prevLayer, prevWidth, prevHeight, x, y, 3);
            }
        }

        buffer.buffer.emplace_back(std::move(newLevel));
        buffer.mipLevels += 1;
        prevWidth = mipWidth;
        prevHeight = mipHeight;
        prevLayer = &buffer.buffer.back().at(0);
    }

    return std::move(buffer);
}

} // namespace slugcat::vrm::mipmap_gen
