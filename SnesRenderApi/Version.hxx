#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

// Standard library
#include <cstdint>

namespace maseya::renderer {
struct Version {
    constexpr Version(uint32_t major, uint32_t minor = 0, uint32_t patch = 0)
            : major(major), minor(minor), patch(patch) {}

    constexpr uint32_t make_version() const {
        return VK_MAKE_VERSION(major, minor, patch);
    }

    constexpr bool operator==(const Version& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    constexpr bool operator!=(const Version& other) const { return !(*this == other); }

    constexpr bool operator<(const Version& other) const {
        return major < other.major ||
               (major == other.major &&
                (minor < other.minor || (minor == other.minor && patch < other.patch)));
    }
    constexpr bool operator>(const Version& other) const { return other < *this; }
    constexpr bool operator<=(const Version& other) const { return !(other < *this); }
    constexpr bool operator>=(const Version& other) const { return !(*this < other); }

    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};
}  // namespace maseya::renderer