#pragma once
#pragma once

// Local headers
#include "BufferHelper.hxx"

namespace maseya::renderer {
class VertexBufferHelper : private BufferHelper {
    VertexBufferHelper(vk::Device device, vk::Queue graphics_queue,
                       vk::CommandPool command_pool, vma::Allocator allocator,
                       size_t count)
            : BufferHelper(device, graphics_queue, command_pool, allocator,
                           vk::BufferUsageFlagBits::eVertexBuffer, count),
              vertex_count_() {}

public:
    template <class Vertex>
    bool update_buffers(const Vertex* data, size_t vertex_count,
                        uint64_t timeout = UINT64_MAX) {
        return update_buffers(data, vertex_count, sizeof(Vertex), timeout);
    }

    bool update_buffers(const void* data, size_t vertex_count, size_t vertex_size,
                        uint64_t timeout = UINT64_MAX) {
        if (BufferHelper::update_buffers(
                    data, vertex_size * static_cast<vk::DeviceSize>(vertex_count),
                    timeout)) {
            vertex_count_ = vertex_count;
            return true;
        }
        return false;
    }

private:
    vk::DeviceSize vertex_count_;

    friend class TestUniformBufferPipeline;
    friend class Surface;
};
}  // namespace maseya::renderer
