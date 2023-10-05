#include "get_graphics_queue_family_index.h"

// Standard library
#include <stdlib.h>

VkResult get_graphics_queue_family_index(VkPhysicalDevice physical_device,
                                         uint32_t* graphics_queue_family_index) {
    VkResult result;

    VkQueueFamilyProperties* queue_family_properties = NULL;

    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                             NULL);

    queue_family_properties = (VkQueueFamilyProperties*)malloc(
            queue_family_count * sizeof(VkQueueFamilyProperties));
    if (queue_family_properties == NULL) {
        result = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto cleanup;
    }

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                             queue_family_properties);

    result = VK_ERROR_FEATURE_NOT_PRESENT;
    for (uint32_t i = 0; i < queue_family_count; i++) {
        if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            *graphics_queue_family_index = i;
            result = VK_SUCCESS;
            break;
        }
    }

cleanup:
    free(queue_family_properties);
    return result;
}