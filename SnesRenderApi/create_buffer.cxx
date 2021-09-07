#include "create_buffer.hxx"

// External APIs
#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

namespace maseya::renderer {
vma::UniqueBuffer create_buffer(vma::Allocator allocator, vk::DeviceSize size,
                                vk::BufferUsageFlags buffer_usage,
                                vma::MemoryUsage memory_usage) {
    vk::BufferCreateInfo buffer_info;
    buffer_info.sharingMode = vk::SharingMode::eExclusive;
    buffer_info.size = size;
    buffer_info.usage = buffer_usage;

    vma::AllocationCreateInfo allocation_info;
    allocation_info.usage = memory_usage;

    auto result = allocator.createBuffer(buffer_info, allocation_info);
    return vma::UniqueBuffer(result.first,
                             vma::UniqueBuffer::deleter(allocator, result.second));
}

std::vector<vma::UniqueBuffer> create_buffers(vma::Allocator allocator,
                                              vk::DeviceSize size,
                                              vk::BufferUsageFlags buffer_usage,
                                              vma::MemoryUsage memory_usage,
                                              size_t count) {
    std::vector<vma::UniqueBuffer> result(count);
    for (vma::UniqueBuffer& buffer : result) {
        buffer = create_buffer(allocator, size, buffer_usage, memory_usage);
    }

    return result;
}
}  // namespace maseya::renderer