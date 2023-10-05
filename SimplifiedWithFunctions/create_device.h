#ifndef CREATE_DEVICE_H
#define CREATE_DEVICE_H

// External APIs
#include <vulkan/vulkan.h>

VkResult create_device(VkPhysicalDevice physical_device, uint32_t queue_family_index,
                       VkDevice* device);

#endif