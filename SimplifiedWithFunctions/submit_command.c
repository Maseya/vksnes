#include "submit_command.h"

VkResult submit_command(VkQueue queue, VkCommandBuffer command_buffer, VkFence fence) {
    VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &command_buffer,
    };

    return vkQueueSubmit(queue, 1, &submit_info, fence);
}
