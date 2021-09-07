#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

// Standard library
#include <cstdint>
#include <optional>
#include <unordered_set>

namespace maseya::renderer {
// Indices of physical device queue families that support desired operations.
class QueueFamilyIndices {
private:
    QueueFamilyIndices(vk::PhysicalDevice physical_device,
                       std::vector<vk::QueueFamilyProperties> families);

public:
    QueueFamilyIndices(vk::PhysicalDevice physical_device)
            : QueueFamilyIndices(physical_device,
                                 physical_device.getQueueFamilyProperties()) {}

    operator bool() const {
        return graphics_family_.has_value() && presentation_family_.has_value();
    }
    bool operator!() const { return !static_cast<bool>(*this); }

    const uint32_t& graphics_family() const { return graphics_family_.value(); }
    const uint32_t& presentation_family() const { return presentation_family_.value(); }

    std::unordered_set<uint32_t> unique_queue_families() const {
        return std::unordered_set<uint32_t>({graphics_family(), presentation_family()});
    }

private:
    std::optional<uint32_t> graphics_family_;
    std::optional<uint32_t> presentation_family_;
};
}  // namespace maseya::renderer