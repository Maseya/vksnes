#pragma once

#include <stdint.h>

namespace maseya::vksnes {
struct PaletteData {
    constexpr static uint32_t max_size = 0x10000;

    uint16_t data[max_size];
    uint32_t size;
};
}  // namespace maseya::vksnes
