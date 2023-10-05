#include "create_pipeline_layout.h"

// Local headers
#include "report.h"

VkResult create_pipeline_layout(VkDevice device, const VkDescriptorSetLayout* layouts,
                                uint32_t layout_count,
                                VkPipelineLayout* pipeline_layout) {
    VkPipelineLayoutCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = layout_count,
            .pSetLayouts = layouts,
    };

    return vkCreatePipelineLayout(device, &create_info, &callbacks, pipeline_layout);
}
