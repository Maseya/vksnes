#ifndef CREATE_BUFFER_H
#define CREATE_BUFFER_H

// External APIs
#include <vulkan/vulkan.h>

VkResult create_buffer(VkPhysicalDevice physical_device, VkDevice device,
                       VkDeviceSize buffer_size, VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags memory_properties, VkBuffer* buffer,
                       VkDeviceMemory* buffer_memory,
                       VkDeviceSize* buffer_allocation_size);

#endif