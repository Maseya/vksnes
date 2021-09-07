#pragma once

// Local headers
#include "Vertex.hxx"
#include "vma_unique_handles.hxx"


namespace maseya::renderer {
class TestUniformBuffers {
private:
    TestUniformBuffers(vk::Device device, vma::Allocator allocator,
                       vk::DescriptorSetLayout uniform_buffer_layout, size_t count);

public:
    TestUniformBuffers(TestUniformBuffers&&) noexcept = default;
    ~TestUniformBuffers() { device_.waitIdle(); }

private:
    vma::Allocator allocator() const { return allocator_; }

private:
    vk::Device device_;
    vma::Allocator allocator_;

    vma::UniqueBuffer staging_vertex_buffer_;

    std::vector<vma::UniqueBuffer> uniform_buffers_;
    vk::UniqueDescriptorPool descriptor_pool_;
    std::vector<vk::DescriptorSet> descriptor_sets_;

    friend class Surface;
    friend class TestBufferRenderTarget;
    friend class TestUniformBufferPipeline;
};
}  // namespace maseya::renderer
