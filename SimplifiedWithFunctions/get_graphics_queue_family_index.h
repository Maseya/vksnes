#ifndef GET_GRAPHICS_QUEUE_FAMILY_INDEX_H
#define GET_GRAPHICS_QUEUE_FAMILY_INDEX_H

// External APIs
#include <vulkan/vulkan.h>

VkResult get_graphics_queue_family_index(VkPhysicalDevice physical_device,
                                         uint32_t* graphics_queue_family_index);

#endif
