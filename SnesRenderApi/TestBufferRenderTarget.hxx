#pragma once

// Local headers
#include "RenderTarget.hxx"
#include "TestUniformBuffers.hxx"

namespace maseya::renderer {
class TestBufferRenderTarget : RenderTarget {
public:
    TestBufferRenderTarget(const RenderTarget& render_target,
                           TestUniformBuffers& surface_buffers)
            : TestBufferRenderTarget(
                      render_target,
                      *surface_buffers.uniform_buffers_[render_target.image_index_],
                      *surface_buffers.descriptor_pool_,
                      surface_buffers.descriptor_sets_[render_target.image_index_]) {}

private:
    TestBufferRenderTarget(const RenderTarget& render_target,
                           vk::Buffer& uniform_buffer,
                           vk::DescriptorPool descriptor_pool,
                           vk::DescriptorSet descriptor_set)
            : RenderTarget(render_target),
              uniform_buffer_(&uniform_buffer),
              descriptor_pool_(descriptor_pool),
              descriptor_set_(descriptor_set) {}

private:
    vk::Buffer* uniform_buffer_;
    vk::DescriptorPool descriptor_pool_;
    vk::DescriptorSet descriptor_set_;
};
}  // namespace maseya::renderer
