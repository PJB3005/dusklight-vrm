#pragma once

// C++ is a language

namespace slugcat::vrm::helpers {

constexpr size_t hashSingle(uint32_t val) {
    return val;
}

// https://stackoverflow.com/a/2595226
template<typename ... Args>
void hashCombine(size_t& seed, const Args& ... v) {
    ([&] {
        seed ^= hashSingle(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    } (), ...);

}

template<typename ... Args>
[[nodiscard]] size_t hashCombine(const Args& ... v) {
    size_t seed = 0;
    hashCombine(seed, v...);
    return seed;
}

}
