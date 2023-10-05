#ifndef CREATE_FRAMEBUFFER_H
#define CREATE_FRAMEBUFFER_H

// External APIs
#include <vulkan/vulkan.h>

VkResult create_framebuffer(VkDevice device, uint32_t width, uint32_t height,
                            VkImageView image_view, VkRenderPass render_pass,
                            VkFramebuffer* framebuffer);

#endif