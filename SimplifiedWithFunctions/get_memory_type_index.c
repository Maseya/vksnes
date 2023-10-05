#include "get_memory_type_index.h"

VkResult get_memory_type_index(VkPhysicalDevice physical_device,
                               const VkMemoryRequirements* memory_requirements,
                               VkMemoryPropertyFlags memory_properties,
                               uint32_t* memory_type_index) {
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device,
                                        &physical_device_memory_properties);

    for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++) {
        if ((memory_requirements->memoryTypeBits & (1 << i)) &&
            (physical_device_memory_properties.memoryTypes[i].propertyFlags &
             memory_properties) == memory_properties) {
            *memory_type_index = i;
            return VK_SUCCESS;
        }
    }

    return VK_ERROR_FEATURE_NOT_PRESENT;
}