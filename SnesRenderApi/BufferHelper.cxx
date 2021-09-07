#include "BufferHelper.hxx"

namespace maseya::renderer {
namespace {
constexpr vk::DeviceSize bit_ceil(vk::DeviceSize v);
std::vector<vk::UniqueSemaphore> create_semaphores(vk::Device device, size_t count);
std::vector<vk::UniqueFence> create_fences(vk::Device device, size_t count);
std::vector<vk::UniqueCommandBuffer> create_command_buffers(
        vk::Device device, vk::CommandPool command_pool, size_t size);
}  // namespace

BufferHelper::BufferHelper(vk::Device device, vk::Queue graphics_queue,
                           vk::CommandPool command_pool, vma::Allocator allocator,
                           vk::BufferUsageFlags usage, size_t count)
        : device_(device),
          graphics_queue_(graphics_queue),
          command_pool_(command_pool),
          command_buffers_(create_command_buffers(device, command_pool, count)),
          allocator_(allocator),
          usage_(usage),
          size_(),
          capacity_(),
          staging_buffer_(),
          buffers_(count),
          copy_fence_handles_(create_fences(device, count)),
          copy_fences_(make_handles(copy_fence_handles_)) {}

bool BufferHelper::update_buffers(const void* data, vk::DeviceSize size,
                                  uint64_t timeout) {
    vk::Result wait_result =
            device_.waitForFences(static_cast<uint32_t>(copy_fences_.size()),
                                  copy_fences_.data(), true, timeout);
    if (wait_result == vk::Result::eTimeout) {
        return false;
    }

    device_.resetFences(copy_fences_.size(), copy_fences_.data());

    if (size > capacity_) {
        capacity_ = bit_ceil(size);
        staging_buffer_ = create_buffer(allocator_, capacity_,
                                        usage_ | vk::BufferUsageFlagBits::eTransferSrc,
                                        vma::MemoryUsage::eCpuToGpu);

        // Hate having to wait. There has to be a better way but I can't think of it
        // right now. The buffers can be in use by several pipelines, not just one. I
        // would need a way of being sure that a buffer is no longer in use by any of
        // them.
        device_.waitIdle();
        buffers_ = create_buffers(allocator_, capacity_,
                                  usage_ | vk::BufferUsageFlagBits::eTransferDst,
                                  vma::MemoryUsage::eGpuOnly, buffers_.size());
    }

    void* dest = staging_buffer_.mapMemory();
    size_ = size;
    std::memcpy(dest, data, size);
    staging_buffer_.unmapMemory();

    for (size_t i = 0; i < buffers_.size(); i++) {
        vk::CommandBufferBeginInfo begin_info;
        begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        command_buffers_[i]->begin(begin_info);

        vk::BufferCopy buffer_copy;
        buffer_copy.srcOffset = 0;
        buffer_copy.dstOffset = 0;
        buffer_copy.size = size;
        command_buffers_[i]->copyBuffer(*staging_buffer_, *buffers_[i], 1,
                                        &buffer_copy);

        command_buffers_[i]->end();

        vk::SubmitInfo submit_info;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &*command_buffers_[i];

        vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eTopOfPipe;
        submit_info.pWaitDstStageMask = &wait_stage;

        graphics_queue_.submit(1, &submit_info, copy_fences_[i]);
    }

    return true;
}

namespace {
// Gets the smallest power of two not less than v.
// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
constexpr vk::DeviceSize bit_ceil(vk::DeviceSize v) {
    --v;

    v |= v >> (1 << 0);
    v |= v >> (1 << 1);
    v |= v >> (1 << 2);
    v |= v >> (1 << 3);
    v |= v >> (1 << 4);
    v |= v >> (1 << 5);

    return ++v;
}

std::vector<vk::UniqueSemaphore> create_semaphores(vk::Device device, size_t count) {
    std::vector<vk::UniqueSemaphore> result(count);
    vk::SemaphoreCreateInfo create_info;
    for (size_t i = 0; i < count; i++) {
        result[i] = device.createSemaphoreUnique(create_info);
    }
    return result;
}

std::vector<vk::UniqueFence> create_fences(vk::Device device, size_t count) {
    std::vector<vk::UniqueFence> result(count);
    vk::FenceCreateInfo create_info;
    create_info.flags = vk::FenceCreateFlagBits::eSignaled;
    for (size_t i = 0; i < count; i++) {
        result[i] = device.createFenceUnique(create_info);
    }
    return result;
}

std::vector<vk::UniqueCommandBuffer> create_command_buffers(
        vk::Device device, vk::CommandPool command_pool, size_t size) {
    vk::CommandBufferAllocateInfo alloc_info;
    alloc_info.level = vk::CommandBufferLevel::ePrimary;
    alloc_info.commandPool = command_pool;
    alloc_info.commandBufferCount = static_cast<uint32_t>(size);
    return device.allocateCommandBuffersUnique(alloc_info);
}
}  // namespace
}  // namespace maseya::renderer