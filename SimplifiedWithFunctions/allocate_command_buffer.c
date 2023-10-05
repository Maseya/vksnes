#include "allocate_command_buffer.h"

VkResult allocate_command_buffer(VkDevice device, VkCommandPool command_pool,
                                 VkCommandBuffer* command_buffer) {
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = command_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };

    return vkAllocateCommandBuffers(device, &command_buffer_allocate_info,
                                    command_buffer);
}