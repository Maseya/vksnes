#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

// Standard library
#include <string>

namespace maseya::renderer {
vk::UniqueShaderModule shader_module_from_file(vk::Device device,
                                               const std::string& path);
}