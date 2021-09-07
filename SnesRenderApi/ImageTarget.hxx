#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

namespace maseya::renderer {
class ImageTarget {
    VkDevice device_;
    VkImage image_;
    VkImageView image_view_;
    VkFramebuffer framebuffer_;
    VkCommandBuffer command_buffer_;

    ~ImageTarget() {
        vkDestroyImageView(device_, image_view_, nullptr);
        vkDestroyFramebuffer(device_, framebuffer_, nullptr);
    }
};
}  // namespace maseya::renderer
