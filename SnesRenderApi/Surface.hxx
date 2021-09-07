#pragma once

// Local headers
#include "Instance.hxx"
#include "RenderTarget.hxx"
#include "SwapChainSupportDetails.hxx"
#include "TestUniformBuffers.hxx"
#include "VertexBufferHelper.hxx"

// External APIs
#include <Windows.h>

#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

// Standard library
#include <optional>
#include <vector>

namespace maseya::renderer {
class Surface {
private:
    constexpr static uint32_t no_timeout = UINT32_MAX;

public:
    Surface(const Instance& instance, HWND hwnd);

    Surface(const Surface&) = delete;
    Surface(Surface&&) noexcept = default;

    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&&) noexcept = default;

    ~Surface() {
        // Image views may still be in use by command buffers, so wait them out before
        // destroying stuff.
        wait_for_fences();
    }

    size_t image_count() const { return images_.size(); }

    RenderTarget acquire_render_target(uint32_t timeout = no_timeout);

    bool present(const RenderTarget& render_target);

    TestUniformBuffers create_test_surface_buffers() {
        return TestUniformBuffers(
                device(), instance_->allocator(),
                *instance_->test_uniform_buffer_pipeline_.uniform_buffer_layout_,
                images_.size());
    }

    VertexBufferHelper create_vertex_buffer_helper() {
        return VertexBufferHelper(*instance_->device_, instance_->graphics_queue_,
                                  *instance_->command_pool_, *instance_->allocator_,
                                  images_.size());
    }

private:
    vk::Device device() const { return instance_->device(); }
    vk::Extent2D extent() const { return details_.extent(); }
    vma::Allocator allocator() const { return instance_->allocator(); }

    void recreate_swap_chain();

    void increment_frame() {
        current_frame_ = (current_frame_ + 1) % max_frames_in_flight_;
    }

    void wait_for_fences() const;

private:
    const Instance* instance_;

    vk::UniqueRenderPass render_pass_;
    vk::UniqueSurfaceKHR surface_;

    SwapChainSupportDetails details_;
    vk::UniqueSwapchainKHR swap_chain_;
    std::vector<vk::Image> images_;
    std::vector<vk::UniqueImageView> image_views_;
    std::vector<vk::UniqueFramebuffer> framebuffers_;
    std::vector<vk::CommandBuffer> command_buffers_;

    uint32_t max_frames_in_flight_;
    uint32_t current_frame_;

    std::vector<vk::UniqueSemaphore> image_available_semaphores_;
    std::vector<vk::UniqueSemaphore> render_finished_semaphores_;
    std::vector<vk::UniqueFence> in_flight_fences_;
    std::vector<vk::Fence> images_in_flight_;

    friend class Instance;
    friend class TestUniformBuffers;
};
}  // namespace maseya::renderer