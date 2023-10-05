#include "begin_command_buffer.h"

VkResult begin_command_buffer(VkCommandBuffer command_buffer) {
    VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    return vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
}
