#ifndef CREATE_PIPELINE_H
#define CREATE_PIPELINE_H

// External APIs
#include <vulkan/vulkan.h>

VkResult create_pipeline(VkDevice device, VkRenderPass render_pass,
                         VkPipelineLayout pipeline_layout, VkShaderModule vertex_shader,
                         VkShaderModule fragment_shader, VkPipeline* pipeline);

#endif