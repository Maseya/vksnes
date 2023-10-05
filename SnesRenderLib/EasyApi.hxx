#pragma once

#include <glm/vec4.hpp>
#include <utility>

#include "Buffer.hxx"
#include "CheckerboardRenderer.hxx"
#include "ChrRenderer.hxx"
#include "CommandBuffer.hxx"
#include "CommandPool.hxx"
#include "DescriptorPoolManager.hxx"
#include "DescriptorSet.hxx"
#include "DescriptorSetManager.hxx"
#include "Device.hxx"
#include "FlatFramebufferFactory.hxx"
#include "GfxRenderer.hxx"
#include "GradientRenderer.hxx"
#include "Image.hxx"
#include "ImageView.hxx"
#include "Instance.hxx"
#include "LineRenderer.hxx"
#include "ManagedSwapchain.hxx"
#include "Map16Renderer.hxx"
#include "ObjRenderer.hxx"
#include "OverlayRenderer.hxx"
#include "PaletteRenderer.hxx"
#include "PhysicalDevice.hxx"
#include "Pipeline.hxx"
#include "PipelineManager.hxx"
#include "PresentationQueue.hxx"
#include "Queue.hxx"
#include "RenderPass.hxx"
#include "RenderPassFactory.hxx"
#include "Sampler.hxx"
#include "Surface.hxx"
#include "Swapchain.hxx"
#include "SwapchainFactory.hxx"
#include "Win32QueueFamilyIndices.hxx"
#include "Win32SurfaceFactory.hxx"

namespace maseya::vksnes::win32 {
class EasyApi {
public:
    EasyApi();
    EasyApi(Instance&& instance);

    template <typename... Args>
    EasyApi(Args&&... args) : EasyApi(Instance(std::forward<Args>(args)...)) {}

    VkDevice device() const { return *device_; }
    VmaAllocator allocator() const { return device_.allocator(); }
    Queue graphics_queue() const { return graphics_queue_; }
    PresentationQueue presentation_queue() const { return presentation_queue_; }

    Image create_image(const VkExtent2D& extent) const;

    ImageView create_image_view(const ImageBase& image) const;

    Image create_dashed_line(const glm::vec4 colors[2], const uint32_t sizes[2]) const;


    DescriptorSet allocate_checker_descriptor_set();

    FlatFramebuffer create_flat_framebuffer(const ImageBase& image) const;

    RenderPass create_flat_render_pass(VkImageLayout final_layout) const;

    CommandBuffer create_command_buffer() const;

    OverlayRenderer create_overlay_renderer();

    CheckerboardRenderer create_checkerboard_renderer();
    LineRenderer create_line_renderer();
    GradientRenderer create_gradient_renderer();
    PaletteRenderer create_palette_renderer();
    GfxRenderer create_gfx_renderer();
    ObjRenderer create_obj_renderer();
    Map16Renderer create_map16_renderer();
    ChrRenderer create_chr_renderer();

    Surface create_surface(HWND hwnd) const;
    Swapchain create_swapchain(VkSurfaceKHR surface) const;
    ManagedSwapchain create_managed_swapchain(VkSurfaceKHR surface) const;
    std::vector<FlatFramebuffer> create_flat_framebuffers(
            const Swapchain& swapchain) const;
    const Pipeline& overlay_pipeline() const { return pipelines_.overlay_pipeline(); }
    const Pipeline& line_pipeline() const { return pipelines_.line_pipeline(); }
    const Pipeline& checker_pipeline() const { return pipelines_.checker_pipeline(); }
    const Pipeline& gradient_pipeline() const { return pipelines_.gradient_pipeline(); }
    const Pipeline& palette_pipeline() const { return pipelines_.palette_pipeline(); }
    const Pipeline& gfx_pipeline(PixelFormat format) const {
        return pipelines_.gfx_pipeline(format);
    }
    const Pipeline& obj_pipeline(bool map16, PixelFormat format) const {
        return pipelines_.obj_pipeline(map16, format);
    }
    const Pipeline& chr_pipeline(PixelFormat format) const {
        return pipelines_.chr_pipeline(format);
    }

private:
    Instance instance_;
    PhysicalDevice physical_device_;
    VkFormat default_surface_format_;
    Win32QueueFamilyIndices queue_family_indices_;
    Device device_;
    RenderPass compatible_render_pass_;
    PipelineManager pipelines_;
    Queue graphics_queue_;
    Queue transfer_queue_;
    PresentationQueue presentation_queue_;
    CommandPool command_pool_;
    DescriptorSetManager descriptor_set_manager_;

    Win32SurfaceFactory surface_factory_;
    SwapchainFactory swapchain_factory_;
    RenderPassFactory render_pass_factory_;
    FlatFramebufferFactory flat_framebuffer_factory_;
};
}  // namespace maseya::vksnes::win32
