#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

namespace maseya::renderer {
class ActiveBuffer {
private:
    ActiveBuffer(vk::DeviceSize size, vk::Buffer buffer, vk::Semaphore copy_semaphore,
                 vk::Semaphore render_semaphore);

private:
    vk::DeviceSize size_;
    vk::Buffer buffer_;
    vk::Semaphore copy_semaphore_;
    vk::Semaphore render_semaphore_;

    friend class BufferHelper;
};
}  // namespace maseya::renderer
