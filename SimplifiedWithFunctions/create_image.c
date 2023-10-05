#include "create_image.h"

// Local headers
#include "get_memory_type_index.h"
#include "report.h"

VkResult create_image(VkPhysicalDevice physical_device, VkDevice device, uint32_t width,
                      uint32_t height, VkFormat format,
                      VkMemoryPropertyFlags memory_properties, VkImage* image,
                      VkDeviceMemory* image_device_memory,
                      VkDeviceSize* image_allocation_size) {
    VkResult result;

    VkImage temp_image = VK_NULL_HANDLE;
    VkDeviceMemory temp_image_device_memory = VK_NULL_HANDLE;

    VkImageCreateInfo image_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .extent.width = width,
            .extent.height = height,
            .extent.depth = 1,
            .mipLevels = 1,
            .arrayLayers = 1,
            .format = format,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .samples = VK_SAMPLE_COUNT_1_BIT,
    };

    result = vkCreateImage(device, &image_create_info, &callbacks, &temp_image);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateImage");

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(device, temp_image, &memory_requirements);

    uint32_t memory_type_index;
    result = get_memory_type_index(physical_device, &memory_requirements,
                                   memory_properties, &memory_type_index);
    if (result < 0) {
        goto cleanup;
    }

    VkMemoryAllocateInfo memory_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memory_requirements.size,
            .memoryTypeIndex = 0,
    };

    result = vkAllocateMemory(device, &memory_allocate_info, &callbacks,
                              &temp_image_device_memory);
    if (result < 0) {
        goto cleanup;
    }

    result = vkBindImageMemory(device, temp_image, temp_image_device_memory, 0);
    if (result < 0) {
        goto cleanup;
    }

    *image = temp_image;
    *image_device_memory = temp_image_device_memory;
    *image_allocation_size = memory_requirements.size;

    temp_image = VK_NULL_HANDLE;
    temp_image_device_memory = VK_NULL_HANDLE;

cleanup:
    vkFreeMemory(device, temp_image_device_memory, &callbacks);
    vkDestroyImage(device, temp_image, &callbacks);

    return result;
}
