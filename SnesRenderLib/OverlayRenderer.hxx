#pragma once

#include <vulkan/vulkan_core.h>

#include "CommandBuffer.hxx"
#include "DescriptorSet.hxx"
#include "DescriptorSetManager.hxx"
#include "Pipeline.hxx"

namespace maseya::vksnes {
class OverlayRenderer {
public:
    OverlayRenderer(VkDevice device, const Pipeline& overlay_pipeline,
                    DescriptorSetManager& descriptor_set_manager);

    void set_image_view(VkImageView image_view, VkSampler sampler);

    void queue_draw(const CommandBuffer& command_buffer) const;

private:
    VkDevice device_;
    VkPipeline overlay_pipeline_;
    VkPipelineLayout overlay_pipeline_layout_;
    DescriptorSet overlay_descriptor_set_;
};
}  // namespace maseya::vksnes
