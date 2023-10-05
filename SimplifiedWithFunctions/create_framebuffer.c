#include "create_framebuffer.h"

// Local headers
#include "report.h"

VkResult create_framebuffer(VkDevice device, uint32_t width, uint32_t height,
                            VkImageView image_view, VkRenderPass render_pass,
                            VkFramebuffer* framebuffer) {
    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = render_pass,
            .attachmentCount = 1,
            .pAttachments = &image_view,
            .width = width,
            .height = height,
            .layers = 1,
    };

    return vkCreateFramebuffer(device, &framebuffer_create_info, &callbacks,
                               framebuffer);
}
