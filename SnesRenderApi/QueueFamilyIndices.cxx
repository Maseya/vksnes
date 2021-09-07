#include "QueueFamilyIndices.hxx"

namespace maseya::renderer {
namespace {
std::optional<uint32_t> get_graphics_family_index(
        const std::vector<vk::QueueFamilyProperties>& families);

std::optional<uint32_t> get_presentation_family_index(
        vk::PhysicalDevice physical_device,
        const std::vector<vk::QueueFamilyProperties>& families);
}  // namespace

QueueFamilyIndices::QueueFamilyIndices(vk::PhysicalDevice physical_device,
                                       std::vector<vk::QueueFamilyProperties> families)
        : graphics_family_(get_graphics_family_index(families)),
          presentation_family_(
                  get_presentation_family_index(physical_device, families)) {}

namespace {
std::optional<uint32_t> get_graphics_family_index(
        const std::vector<vk::QueueFamilyProperties>& families) {
    for (size_t i = 0; i < families.size(); i++) {
        if (families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            return static_cast<uint32_t>(i);
        }
    }
    return std::nullopt;
}

std::optional<uint32_t> get_presentation_family_index(
        vk::PhysicalDevice physical_device,
        const std::vector<vk::QueueFamilyProperties>& families) {
    for (size_t i = 0; i < families.size(); i++) {
        if (physical_device.getWin32PresentationSupportKHR(static_cast<uint32_t>(i))) {
            return static_cast<uint32_t>(i);
        }
    }
    return std::nullopt;
}
}  // namespace
}  // namespace maseya::renderer