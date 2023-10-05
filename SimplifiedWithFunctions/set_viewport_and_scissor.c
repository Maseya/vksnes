#include "set_viewport_and_scissor.h"

void set_viewport_and_scissor(VkCommandBuffer command_buffer, uint32_t width,
                              uint32_t height) {
    VkViewport viewport = {
            .width = width,
            .height = height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {
            .offset = {0, 0},
            .extent = {width, height},
    };
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}