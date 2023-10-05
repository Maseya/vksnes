#ifndef CREATE_PIPELINE_LAYOUT_H
#define CREATE_PIPELINE_LAYOUT_H

// External APIs
#include <vulkan/vulkan.h>

VkResult create_pipeline_layout(VkDevice device, const VkDescriptorSetLayout* layouts,
                                uint32_t layout_count,
                                VkPipelineLayout* pipeline_layout);

#endif