#ifndef GET_MEMORY_TYPE_INDEX_H
#define GET_MEMORY_TYPE_INDEX_H

// External APIs
#include <vulkan/vulkan.h>

VkResult get_memory_type_index(VkPhysicalDevice physical_device,
                               const VkMemoryRequirements* memory_requirements,
                               VkMemoryPropertyFlags memory_properties,
                               uint32_t* memory_type_index);

#endif
