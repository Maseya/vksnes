#ifndef CREATE_IMAGE_H
#define CREATE_IMAGE_H

// External APIs
#include <vulkan/vulkan.h>

VkResult create_image(VkPhysicalDevice physical_device, VkDevice device, uint32_t width,
                      uint32_t height, VkFormat format,
                      VkMemoryPropertyFlags memory_properties, VkImage* image,
                      VkDeviceMemory* image_device_memory,
                      VkDeviceSize* image_allocation_size);

#endif