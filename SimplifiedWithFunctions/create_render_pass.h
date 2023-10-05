#ifndef CREATE_RENDER_PASS_H
#define CREATE_RENDER_PASS_H

// External APIs
#include <vulkan/vulkan.h>

VkResult create_render_pass(VkDevice device, VkFormat format,
                            VkImageLayout initial_layout, VkImageLayout final_layout,
                            VkRenderPass* render_pass);

#endif