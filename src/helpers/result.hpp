#pragma once

#include <stdexcept>
#include "mods/api.h"

namespace slugcat::vrm::helpers {

inline void checkResult(ModResult const res) {
    if (res != MOD_OK) {
        throw std::runtime_error("Service call failed");
    }
}

}  // namespace slugcat::vrm::helpers