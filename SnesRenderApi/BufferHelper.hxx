#pragma once

// Local headers
#include "ActiveBuffer.hxx"
#include "create_buffer.hxx"
#include "make_handles.hxx"

// External APIs
#include <vulkan/vulkan.hpp>

// Standard library
#include <vector>

namespace maseya::renderer {
class BufferHelper {
private:
    BufferHelper(vk::Device device, vk::Queue graphics_queue,
                 vk::CommandPool command_pool, vma::Allocator allocator,
                 vk::BufferUsageFlags usage, size_t count);

public:
    bool update_buffers(const void* data, vk::DeviceSize size,
                        uint64_t timeout = UINT64_MAX);

private:
    vk::Device device_;
    vk::Queue graphics_queue_;
    vk::CommandPool command_pool_;
    std::vector<vk::UniqueCommandBuffer> command_buffers_;
    vma::Allocator allocator_;
    vk::BufferUsageFlags usage_;
    vk::DeviceSize size_;
    vk::DeviceSize capacity_;
    vma::UniqueBuffer staging_buffer_;
    std::vector<vma::UniqueBuffer> buffers_;
    std::vector<vk::UniqueFence> copy_fence_handles_;
    std::vector<vk::Fence> copy_fences_;

    friend class Surface;
    friend class VertexBufferHelper;
    friend class TestUniformBufferPipeline;
};
}  // namespace maseya::renderer
