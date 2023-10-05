#ifndef ALLOCATE_COMMAND_BUFFER_H
#define ALLOCATE_COMMAND_BUFFER_H

// External APIs
#include <vulkan/vulkan.h>

VkResult allocate_command_buffer(VkDevice device, VkCommandPool command_pool,
                                 VkCommandBuffer* command_buffer);

#endif