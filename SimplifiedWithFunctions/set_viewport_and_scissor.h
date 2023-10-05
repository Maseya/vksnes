#ifndef SET_VIEWPORT_H
#define SET_VIEWPORT_H

// External APIs
#include <vulkan/vulkan.h>

void set_viewport_and_scissor(VkCommandBuffer command_buffer, uint32_t width,
                              uint32_t height);

#endif
