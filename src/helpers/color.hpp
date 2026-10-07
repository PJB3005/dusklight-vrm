#pragma once

#include "glm/glm.hpp"

namespace slugcat::vrm::helpers {

constexpr glm::vec4 convertColor(GXColorS10 const& color) noexcept {
    return glm::vec4{
        color.r / 255.0,
        color.g / 255.0,
        color.b / 255.0,
        color.a / 255.0,
    };
}

constexpr glm::vec4 convertColor(GXColor const& color) noexcept {
    return glm::vec4{
        color.r / 255.0,
        color.g / 255.0,
        color.b / 255.0,
        color.a / 255.0,
    };
}

}  // namespace slugcat::vrm::helpers
