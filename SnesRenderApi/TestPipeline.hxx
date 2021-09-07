#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

// Local headers
#include "RenderTarget.hxx"

namespace maseya::renderer {
// A trivial pipeline for testing API features.
class TestPipeline {
private:
    TestPipeline(vk::Device device, vk::Format format);

    void render(RenderTarget& render_target) const;

    vk::RenderPass render_pass() const { return *render_pass_; }

private:
    vk::UniqueRenderPass render_pass_;
    vk::UniquePipelineLayout pipeline_layout_;
    vk::UniquePipeline graphics_pipeline_;

    friend class Instance;
};
}  // namespace maseya::renderer