#pragma once

#include <vkbase/Buffer.hxx>
#include <vkbase/CommandBuffer.hxx>
#include <vkbase/DescriptorSet.hxx>
#include <vkbase/DescriptorSetManager.hxx>
#include <vkbase/Device.hxx>
#include <vkbase/Pipeline.hxx>

#include "CheckerboardPattern.hxx"

namespace maseya::vksnes {
class CheckerboardRenderer {
public:
    CheckerboardRenderer(VmaAllocator allocator,
                         const vkbase::Pipeline& checker_pipeline,
                         vkbase::DescriptorSetManager& descriptor_set_manager);

    CheckerboardRenderer(const CheckerboardRenderer&) = delete;
    CheckerboardRenderer(CheckerboardRenderer&&) noexcept = default;

    CheckerboardRenderer& operator=(const CheckerboardRenderer&) = delete;
    CheckerboardRenderer& operator=(CheckerboardRenderer&&) noexcept = default;

    CheckerboardPattern& checkerboard_pattern() {
        return *static_cast<CheckerboardPattern*>(checker_buffer_.data());
    }
    const CheckerboardPattern& checkerboard_pattern() const {
        return *static_cast<const CheckerboardPattern*>(checker_buffer_.data());
    }

    void queue_draw(const vkbase::CommandBuffer& command_buffer) const;

private:
    VkPipeline checker_pipeline_;
    VkPipelineLayout checker_pipeline_layout_;
    vkbase::Buffer checker_buffer_;
    vkbase::DescriptorSet checker_descriptor_set_;
};
}  // namespace maseya::vksnes
