#ifndef SUBMIT_COMMAND_H
#define SUBMIT_COMMAND_H

// External APIs
#include <vulkan/vulkan.h>

VkResult submit_command(VkQueue queue, VkCommandBuffer command_buffer, VkFence fence);

#endif