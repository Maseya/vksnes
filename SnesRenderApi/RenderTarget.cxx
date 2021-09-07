#include "RenderTarget.hxx"

// Local headers
#include "VulkanError.hxx"

namespace maseya::renderer {
RenderTarget::RenderTarget(vk::Device device, vk::Queue graphics_queue,
                           uint32_t image_index, vk::Extent2D extent,
                           vk::Framebuffer framebuffer,
                           vk::CommandBuffer command_buffer,
                           vk::Semaphore image_available_semaphore,
                           vk::Semaphore render_finished_semaphore,
                           vk::Fence image_in_flight)
        : device_(device),
          graphics_queue_(graphics_queue),
          image_index_(image_index),
          extent_(extent),
          framebuffer_(framebuffer),
          command_buffer_(command_buffer),
          image_available_semaphore_(image_available_semaphore),
          render_finished_semaphore_(render_finished_semaphore),
          image_in_flight_(image_in_flight),
          viewport_(0, 0, static_cast<float>(extent_.width),
                    static_cast<float>(extent_.height), 0, 1),
          scissor_({}, extent_),
          command_buffer_begin_info_(),
          wait_stage_(vk::PipelineStageFlagBits::eColorAttachmentOutput) {}

void RenderTarget::begin(vk::RenderPass render_pass,
                         const vk::ClearColorValue& clear_color) {
    vk::ClearValue clear_value = clear_color;

    vk::RenderPassBeginInfo render_pass_begin_info;
    render_pass_begin_info.renderPass = render_pass;
    render_pass_begin_info.framebuffer = framebuffer_;
    render_pass_begin_info.renderArea.offset = vk::Offset2D(0, 0);
    render_pass_begin_info.renderArea.extent = extent_;
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_value;

    command_buffer_.begin(command_buffer_begin_info_);
    command_buffer_.setViewport(0, 1, &viewport_);
    command_buffer_.setScissor(0, 1, &scissor_);
    command_buffer_.beginRenderPass(render_pass_begin_info,
                                    vk::SubpassContents::eInline);
}

void RenderTarget::end() {
    command_buffer_.endRenderPass();
    command_buffer_.end();

    vk::Result reset_fence_result = device_.resetFences(1, &image_in_flight_);
    if (reset_fence_result == vk::Result::eErrorOutOfDeviceMemory) {
        throw VulkanError("Could not reset fences. Device ran out of memory.");
    }

    vk::SubmitInfo submit_info(1, &image_available_semaphore_, &wait_stage_, 1,
                               &command_buffer_, 1, &render_finished_semaphore_);

    vk::Result draw_result = graphics_queue_.submit(1, &submit_info, image_in_flight_);
    if (draw_result != vk::Result::eSuccess) {
        throw VulkanError("Draw command failed!");
    }
}
}  // namespace maseya::renderer