#include "create_device.h"

// Local headers
#include "create_instance.h"
#include "report.h"

VkResult create_device(VkPhysicalDevice physical_device, uint32_t queue_family_index,
                       VkDevice* device) {
    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo device_queue_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_family_index,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority,
    };

    VkDeviceCreateInfo device_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &device_queue_create_info,
            .enabledLayerCount = 1,
            .ppEnabledLayerNames = layers,
            .enabledExtensionCount = 0,
            .ppEnabledExtensionNames = NULL,
    };

    return vkCreateDevice(physical_device, &device_create_info, &callbacks, device);
}