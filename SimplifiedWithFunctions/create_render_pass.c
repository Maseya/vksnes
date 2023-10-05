#include "create_render_pass.h"

// Local headers
#include "report.h"

VkResult create_render_pass(VkDevice device, VkFormat format,
                            VkImageLayout initial_layout, VkImageLayout final_layout,
                            VkRenderPass* render_pass) {
    VkAttachmentDescription color_attachment_description = {
            .format = format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = initial_layout == VK_IMAGE_LAYOUT_UNDEFINED
                              ? VK_ATTACHMENT_LOAD_OP_CLEAR
                              : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = initial_layout,
            .finalLayout = final_layout,
    };

    VkAttachmentReference color_attachment_reference = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass_description = {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment_reference,
    };

    VkRenderPassCreateInfo render_pass_create_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &color_attachment_description,
            .subpassCount = 1,
            .pSubpasses = &subpass_description,
    };

    return vkCreateRenderPass(device, &render_pass_create_info, &callbacks,
                              render_pass);
}