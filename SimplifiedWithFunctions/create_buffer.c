#include "create_buffer.h"

// Local headers
#include "get_memory_type_index.h"
#include "report.h"

extern VkAllocationCallbacks callbacks;

VkResult create_buffer(VkPhysicalDevice physical_device, VkDevice device,
                       VkDeviceSize buffer_size, VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags memory_properties, VkBuffer* buffer,
                       VkDeviceMemory* buffer_memory,
                       VkDeviceSize* buffer_allocation_size) {
    VkResult result;

    VkBuffer temp_buffer = VK_NULL_HANDLE;
    VkDeviceMemory temp_buffer_memory = VK_NULL_HANDLE;

    VkBufferCreateInfo buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = buffer_size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    result = vkCreateBuffer(device, &buffer_create_info, &callbacks, &temp_buffer);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateBuffer");

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device, temp_buffer, &memory_requirements);

    uint32_t memory_type_index;
    result = get_memory_type_index(physical_device, &memory_requirements,
                                   memory_properties, &memory_type_index);
    if (result < 0) {
        goto cleanup;
    }

    VkMemoryAllocateInfo memory_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memory_requirements.size,
            .memoryTypeIndex = memory_type_index,
    };

    result = vkAllocateMemory(device, &memory_allocate_info, &callbacks,
                              &temp_buffer_memory);
    if (result < 0) {
        goto cleanup;
    }

    result = vkBindBufferMemory(device, temp_buffer, temp_buffer_memory, 0);
    if (result < 0) {
        goto cleanup;
    }

    *buffer = temp_buffer;
    *buffer_memory = temp_buffer_memory;
    *buffer_allocation_size = memory_requirements.size;

    temp_buffer = VK_NULL_HANDLE;
    temp_buffer_memory = VK_NULL_HANDLE;

cleanup:
    vkFreeMemory(device, temp_buffer_memory, &callbacks);
    vkDestroyBuffer(device, temp_buffer, &callbacks);

    return result;
}