#pragma once

#include <stdint.h>

namespace maseya::vksnes {
struct Map16Data {
    constexpr static uint32_t max_size = 0x40000;

    uint8_t data[max_size];
    uint32_t size;
};
}  // namespace maseya::vksnes
