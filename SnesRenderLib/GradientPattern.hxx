#pragma once

#include <cstdint>
#include <glm/vec4.hpp>

namespace maseya::vksnes {
struct GradientPattern {
    glm::vec4 colors[2];
    float angle;
    uint32_t steps;
};
}  // namespace maseya::vksnes
