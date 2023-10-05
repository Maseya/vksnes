#pragma once

#include <cstdint>
#include <glm/vec2.hpp>

namespace maseya::vksnes {
struct gfx_view {
    glm::uvec2 size;
    uint32_t palette_start_index;
};
}  // namespace maseya::vksnes
