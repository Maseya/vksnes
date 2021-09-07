#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

namespace maseya::renderer {
class RenderTarget {
private:
    RenderTarget() = default;
    RenderTarget(vk::Device device, vk::Queue graphics_queue, uint32_t image_index,
                 vk::Extent2D extent, vk::Framebuffer framebuffer,
                 vk::CommandBuffer command_buffer,
                 vk::Semaphore image_available_semaphore,
                 vk::Semaphore render_finished_semaphore, vk::Fence image_in_flight);

public:
    operator bool() const { return extent_.width != 0 && extent_.height != 0; }
    bool operator!() const { return !static_cast<bool>(*this); }

    const uint32_t& image_index() const { return image_index_; }

private:
    void begin(vk::RenderPass render_pass, const vk::ClearColorValue& clear_color);
    void end();

    const vk::Extent2D& extent() const { return extent_; }
    vk::Framebuffer framebuffer() const { return framebuffer_; }
    vk::CommandBuffer command_buffer() const { return command_buffer_; }

    const vk::Semaphore& image_available_semaphore() const {
        return image_available_semaphore_;
    }
    const vk::Semaphore& render_finished_semaphore() const {
        return render_finished_semaphore_;
    }
    const vk::Fence& image_in_flight() const { return image_in_flight_; }

private:
    vk::Device device_;
    vk::Queue graphics_queue_;

    uint32_t image_index_;
    vk::Extent2D extent_;
    vk::Framebuffer framebuffer_;
    vk::CommandBuffer command_buffer_;
    vk::Semaphore image_available_semaphore_;
    vk::Semaphore render_finished_semaphore_;
    vk::Fence image_in_flight_;

    vk::Viewport viewport_;
    vk::Rect2D scissor_;
    vk::CommandBufferBeginInfo command_buffer_begin_info_;
    vk::PipelineStageFlags wait_stage_;

    friend class Surface;
    friend class TestPipeline;
    friend class TestUniformBufferPipeline;
    friend class TestBufferRenderTarget;
    friend class PalettePipeline;
};
}  // namespace maseya::renderer