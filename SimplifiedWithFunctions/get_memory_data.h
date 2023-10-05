#ifndef GET_MEMORY_DATA_H
#define GET_MEMORY_DATA_H

// External APIs
#include <vulkan/vulkan.h>

VkResult get_memory_data(VkDevice device, VkDeviceMemory memory, VkDeviceSize size,
                         void* data);

#endif