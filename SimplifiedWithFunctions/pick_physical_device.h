#ifndef PICK_PHYSICAL_DEVICE_H
#define PICK_PHYSICAL_DEVICE_H

// External APIs
#include <vulkan/vulkan.h>

VkResult pick_physical_device(VkInstance instance, VkPhysicalDevice* physical_device);

#endif
