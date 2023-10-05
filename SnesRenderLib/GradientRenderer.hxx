#pragma once

#include "Buffer.hxx"
#include "CommandBuffer.hxx"
#include "DescriptorSet.hxx"
#include "DescriptorSetManager.hxx"
#include "Device.hxx"
#include "GradientPattern.hxx"
#include "Pipeline.hxx"

namespace maseya::vksnes {
class GradientRenderer {
public:
    GradientRenderer(VmaAllocator allocator, const Pipeline& gradient_pipeline,
                     DescriptorSetManager& descriptor_set_manager);

    VkExtent2D& frame_size() {
        return *static_cast<VkExtent2D*>(frame_size_buffer_.data());
    }
    const VkExtent2D& frame_size() const {
        return *static_cast<const VkExtent2D*>(frame_size_buffer_.data());
    }

    GradientPattern& gradient_pattern() {
        return *static_cast<GradientPattern*>(gradient_buffer_.data());
    }
    const GradientPattern& gradient_pattern() const {
        return *static_cast<const GradientPattern*>(gradient_buffer_.data());
    }

    void queue_draw(const CommandBuffer& command_buffer) const;

private:
    VkPipeline gradient_pipeline_;
    VkPipelineLayout gradient_pipeline_layout_;
    Buffer frame_size_buffer_;
    Buffer gradient_buffer_;
    DescriptorSet gradient_descriptor_set_;
};
}  // namespace maseya::vksnes
