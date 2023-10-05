#ifndef BEGIN_RENDER_PASS_H
#define BEGIN_RENDER_PASS_H

// External APIs
#include <vulkan/vulkan.h>

void begin_render_pass(VkCommandBuffer command_buffer, VkRenderPass render_pass,
                       VkFramebuffer framebuffer, uint32_t width, uint32_t height,
                       const float clear_color[4]);

#endif