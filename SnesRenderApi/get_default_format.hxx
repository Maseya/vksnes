#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

namespace maseya::renderer {
vk::Format get_default_format(vk::Instance instance,
                              vk::PhysicalDevice physical_device);
}