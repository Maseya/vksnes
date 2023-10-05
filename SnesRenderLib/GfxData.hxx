#pragma once

#include <stdint.h>

namespace maseya::vksnes {
struct GfxData {
    constexpr static uint32_t max_size = 0x20000;

    uint8_t data[max_size];
    uint32_t size;
};
}  // namespace maseya::vksnes
