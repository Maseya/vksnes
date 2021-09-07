#include "ActiveBuffer.hxx"

namespace maseya::renderer {
ActiveBuffer::ActiveBuffer(vk::DeviceSize size, vk::Buffer buffer,
                           vk::Semaphore copy_semaphore, vk::Semaphore render_semaphore)
        : size_(size),
          buffer_(buffer),
          copy_semaphore_(copy_semaphore),
          render_semaphore_(render_semaphore) {}
}  // namespace maseya::renderer