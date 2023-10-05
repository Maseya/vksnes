#ifndef CREATE_COMMAND_POOL_H
#define CREATE_COMMAND_POOL_H

// External APIs
#include <vulkan/vulkan.h>

VkResult create_command_pool(VkDevice device, uint32_t queue_family_index,
                             VkCommandPool* command_pool);

#endif