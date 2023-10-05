#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace maseya::vksnes {
struct CheckerboardPattern {
    glm::vec4 colors[2];
    glm::vec2 size;
};
}  // namespace maseya::vksnes
