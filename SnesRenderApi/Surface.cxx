#include "Surface.hxx"

// Local headers
#include "Instance.hxx"
#include "TestUniformBuffers.hxx"
#include "VulkanError.hxx"

// External headers
#include <Windows.h>

namespace maseya::renderer {
namespace {
vk::UniqueSurfaceKHR create_surface(vk::Instance instance,
                                    vk::PhysicalDevice physical_device,
                                    uint32_t presentation_queue_index, HWND hwnd);

std::vector<vk::UniqueImageView> create_image_views(
        vk::Device device, const std::vector<vk::Image>& images, vk::Format format);

std::vector<vk::UniqueSemaphore> create_semaphores(vk::Device device, size_t count);

std::vector<vk::UniqueFence> create_fences(vk::Device device, size_t count);

std::vector<vk::CommandBuffer> create_command_buffers(vk::Device device,
                                                      vk::CommandPool command_pool,
                                                      size_t size);

vk::UniqueRenderPass create_render_pass(vk::Device device, vk::Format format);

std::vector<vk::UniqueFramebuffer> create_framebuffers(
        vk::Device device, const std::vector<vk::UniqueImageView>& image_views,
        vk::RenderPass render_pass, const vk::Extent2D& extent);
}  // namespace

Surface::Surface(const Instance& instance, HWND hwnd)
        : instance_(&instance),
          render_pass_(
                  create_render_pass(instance.device(), instance.default_format())),
          surface_(create_surface(instance.instance(), instance.physical_device(),
                                  instance.presentation_queue_index(), hwnd)),
          details_(instance.physical_device(), *surface_),
          swap_chain_(instance.device().createSwapchainKHRUnique(
                  details_.swap_chain_create_info())),
          images_(instance.device().getSwapchainImagesKHR(*swap_chain_)),
          image_views_(
                  create_image_views(instance.device(), images_, details_.format())),
          framebuffers_(create_framebuffers(instance.device(), image_views_,
                                            *render_pass_, details_.extent())),
          command_buffers_(create_command_buffers(device(), instance.command_pool(),
                                                  images_.size())),
          max_frames_in_flight_(static_cast<uint32_t>(
                  images_.size() - details_.capabilities().minImageCount + 1)),
          current_frame_(0),
          image_available_semaphores_(
                  create_semaphores(instance.device(), max_frames_in_flight_)),
          render_finished_semaphores_(
                  create_semaphores(instance.device(), max_frames_in_flight_)),
          in_flight_fences_(create_fences(instance.device(), max_frames_in_flight_)),
          images_in_flight_(images_.size(), nullptr) {}

RenderTarget Surface::acquire_render_target(uint32_t timeout) {
    if (!instance_->wait_for_fence(*in_flight_fences_[current_frame_], timeout)) {
        return RenderTarget();
    }

    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(
            device(), *swap_chain_, 0, *image_available_semaphores_[current_frame_],
            NULL, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swap_chain();
        return RenderTarget();
    }

    if (result != VK_SUCCESS) {
        throw VulkanError("Image not available when expected.");
    }

    // Check if a previous frame is using this image.
    if (images_in_flight_[image_index]) {
        instance_->wait_for_fence(images_in_flight_[image_index]);
    }

    images_in_flight_[image_index] = *in_flight_fences_[current_frame_];

    return RenderTarget(device(), instance_->graphics_queue(), image_index, extent(),
                        *framebuffers_[image_index], command_buffers_[image_index],
                        *image_available_semaphores_[current_frame_],
                        *render_finished_semaphores_[current_frame_],
                        *in_flight_fences_[current_frame_]);
}

bool Surface::present(const RenderTarget& render_target) {
    if (!render_target) {
        return true;
    }

    vk::PresentInfoKHR present_info;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_target.render_finished_semaphore();
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &(*swap_chain_);
    present_info.pImageIndices = &render_target.image_index();

    VkResult result =
            vkQueuePresentKHR(instance_->presentation_queue(),
                              &static_cast<const VkPresentInfoKHR&>(present_info));

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreate_swap_chain();
        return false;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    increment_frame();
    return true;
}

void Surface::wait_for_fences() const {
    for (const vk::UniqueFence& fence : in_flight_fences_) {
        instance_->wait_for_fence(*fence);
    }
}

void Surface::recreate_swap_chain() {
    details_ = SwapChainSupportDetails(instance_->physical_device(), *surface_);
    if (extent().width == 0 || extent().height == 0) {
        return;
    }

    // We cannot destroy old image views while they are in use by command buffers.
    wait_for_fences();

    swap_chain_ = device().createSwapchainKHRUnique(
            details_.swap_chain_create_info(*swap_chain_));
    images_ = device().getSwapchainImagesKHR(*swap_chain_);
    image_views_ = create_image_views(device(), images_, details_.format());
    framebuffers_ =
            create_framebuffers(device(), image_views_, *render_pass_, extent());
}

namespace {
vk::UniqueSurfaceKHR create_surface(vk::Instance instance,
                                    vk::PhysicalDevice physical_device,
                                    uint32_t presentation_queue_index, HWND hwnd) {
    vk::Win32SurfaceCreateInfoKHR create_info;
    create_info.hinstance =
            reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hwnd, GWLP_HINSTANCE));
    create_info.hwnd = hwnd;
    vk::UniqueSurfaceKHR surface = instance.createWin32SurfaceKHRUnique(create_info);

    if (!physical_device.getSurfaceSupportKHR(presentation_queue_index, *surface)) {
        throw VulkanError("Surface not supported by physical device.");
    }

    return surface;
}

// Create a 2D image view for a 2D image.
vk::UniqueImageView create_image_view(vk::Device device, vk::Image image,
                                      vk::Format format);

std::vector<vk::UniqueImageView> create_image_views(
        vk::Device device, const std::vector<vk::Image>& images, vk::Format format) {
    std::vector<vk::UniqueImageView> result;

    // Create an image view for each image.
    for (vk::Image image : images) {
        result.push_back(create_image_view(device, image, format));
    }

    return result;
}

vk::UniqueImageView create_image_view(vk::Device device, vk::Image image,
                                      vk::Format format) {
    vk::ImageViewCreateInfo create_info;
    create_info.image = image;
    create_info.viewType = vk::ImageViewType::e2D;
    create_info.format = format;
    create_info.components.r = vk::ComponentSwizzle::eIdentity;
    create_info.components.g = vk::ComponentSwizzle::eIdentity;
    create_info.components.b = vk::ComponentSwizzle::eIdentity;
    create_info.components.a = vk::ComponentSwizzle::eIdentity;
    create_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    return device.createImageViewUnique(create_info);
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

std::vector<vk::CommandBuffer> create_command_buffers(vk::Device device,
                                                      vk::CommandPool command_pool,
                                                      size_t size) {
    vk::CommandBufferAllocateInfo alloc_info;
    alloc_info.commandPool = command_pool;
    alloc_info.level = vk::CommandBufferLevel::ePrimary;
    alloc_info.commandBufferCount = static_cast<uint32_t>(size);
    return device.allocateCommandBuffers(alloc_info);
}

vk::UniqueRenderPass create_render_pass(vk::Device device, vk::Format format) {
    vk::AttachmentDescription color_attachment;
    color_attachment.format = format;
    color_attachment.samples = vk::SampleCountFlagBits::e1;
    color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
    color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
    color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    color_attachment.initialLayout = vk::ImageLayout::eUndefined;
    color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    vk::SubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = static_cast<vk::AccessFlagBits>(0);
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo create_info;
    create_info.attachmentCount = 1;
    create_info.pAttachments = &color_attachment;
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass;
    create_info.dependencyCount = 1;
    create_info.pDependencies = &dependency;

    return device.createRenderPassUnique(create_info);
}

std::vector<vk::UniqueFramebuffer> create_framebuffers(
        vk::Device device, const std::vector<vk::UniqueImageView>& image_views,
        vk::RenderPass compatible_render_pass, const vk::Extent2D& extent) {
    std::vector<vk::UniqueFramebuffer> result(image_views.size());
    for (size_t i = 0; i < image_views.size(); i++) {
        vk::FramebufferCreateInfo framebuffer_info;
        framebuffer_info.renderPass = compatible_render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = &*image_views[i];
        framebuffer_info.width = extent.width;
        framebuffer_info.height = extent.height;
        framebuffer_info.layers = 1;

        result[i] = device.createFramebufferUnique(framebuffer_info);
    }

    return result;
}
}  // namespace
}  // namespace maseya::renderer