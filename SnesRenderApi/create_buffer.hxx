#pragma once

// Local headers
#include "vma_unique_handles.hxx"

// External APIs
#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

// Standard library
#include <vector>

namespace maseya::renderer {
vma::UniqueBuffer create_buffer(vma::Allocator allocator, vk::DeviceSize size,
                                vk::BufferUsageFlags buffer_usage,
                                vma::MemoryUsage memory_usage);

template <class VertexT>
vma::UniqueBuffer create_vertex_buffer(vma::Allocator allocator, vk::DeviceSize count) {
    return create_buffer(allocator, count * sizeof(VertexT),
                         vk::BufferUsageFlagBits::eVertexBuffer,
                         vma::MemoryUsage::eGpuToCpu);
}

std::vector<vma::UniqueBuffer> create_buffers(vma::Allocator allocator,
                                              vk::DeviceSize size,
                                              vk::BufferUsageFlags buffer_usage,
                                              vma::MemoryUsage memory_usage,
                                              size_t count);
}  // namespace maseya::renderer