#pragma once

// Local headers
#include "RenderTarget.hxx"
#include "TestUniformBuffers.hxx"
#include "Vertex.hxx"
#include "VertexBufferHelper.hxx"

// External APIs
#include <vulkan/vulkan.hpp>

// Standard library
#include <cstdlib>
#include <vector>

namespace maseya::renderer {
// A trivial pipeline for testing API features.
class TestUniformBufferPipeline {
public:
    TestUniformBufferPipeline(vk::Device device, vk::Format format);

    void render(RenderTarget& render_target, const TestUniformBuffers& surface_buffers,
                const VertexBufferHelper& vertex_buffer, float angle) const;

private:
    vk::RenderPass render_pass() const { return *render_pass_; }

private:
    vk::UniqueRenderPass render_pass_;
    vk::UniqueDescriptorSetLayout uniform_buffer_layout_;
    vk::UniquePipelineLayout pipeline_layout_;
    vk::UniquePipeline graphics_pipeline_;

    friend class Surface;
    friend class TestUniformBuffers;
};
}  // namespace maseya::renderer