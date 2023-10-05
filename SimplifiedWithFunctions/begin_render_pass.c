#include "begin_render_pass.h"

void begin_render_pass(VkCommandBuffer command_buffer, VkRenderPass render_pass,
                       VkFramebuffer framebuffer, uint32_t width, uint32_t height,
                       const float clear_color[4]) {
    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = render_pass,
            .framebuffer = framebuffer,
            .renderArea.offset = {0, 0},
            .renderArea.extent = {width, height},
            .clearValueCount = 1,
            .pClearValues = (const VkClearValue*)clear_color,
    };

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);
}
