#pragma once

// Local headers
#include "RenderTarget.hxx"

// External APIs
#include <vulkan/vulkan.hpp>

namespace maseya::renderer {
class PalettePipeline {
public:
    PalettePipeline(vk::Device device, vk::Format format);

    void render(RenderTarget& render_target) const;

private:
    vk::UniqueRenderPass render_pass_;
    vk::UniqueDescriptorSetLayout descriptor_set_layout_;
    vk::UniquePipelineLayout pipeline_layout_;
    vk::UniquePipeline graphics_pipeline_;
};
}  // namespace maseya::renderer