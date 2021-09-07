#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

// Standard library
#include <vector>

namespace maseya::renderer {
template <class Type, class Dispatch = VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>
std::vector<Type> make_handles(
        const std::vector<vk::UniqueHandle<Type, Dispatch>>& unique_handles) {
    std::vector<Type> result(unique_handles.size());
    for (size_t i = 0; i < unique_handles.size(); i++) {
        result[i] = *unique_handles[i];
    }

    return result;
}
}  // namespace maseya::renderer