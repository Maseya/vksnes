#include "EasyApi.hxx"

#include "vulkan_helper.hxx"
#include "vulkan_win32_helper.hxx"

namespace maseya::vksnes::win32 {
EasyApi::EasyApi() : EasyApi(Instance()) {}

EasyApi::EasyApi(Instance&& instance)
        : instance_(std::move(instance)),
          physical_device_(*instance_, pick_physical_device(*instance_)),
          default_surface_format_(get_default_format(*instance_, *physical_device_)),
          queue_family_indices_(*instance_, *physical_device_),
          device_(*instance_, *physical_device_,
                  queue_family_indices_.queue_family_indices(),
                  default_surface_format_),
          compatible_render_pass_(*device_, device_.default_image_format()),
          pipelines_(*device_, *compatible_render_pass_),
          graphics_queue_(*device_, queue_family_indices_.graphics_family()),
          transfer_queue_(*device_, queue_family_indices_.transfer_family()),
          presentation_queue_(*device_, queue_family_indices_.presentation_family()),
          command_pool_(graphics_queue_.device(), graphics_queue_.queue_family_index()),
          descriptor_set_manager_(),
          surface_factory_(*instance_),
          swapchain_factory_(*physical_device_, *device_),
          render_pass_factory_(*device_, device_.default_image_format(),
                               VK_FORMAT_UNDEFINED),
          flat_framebuffer_factory_(*device_, *compatible_render_pass_) {}

Image EasyApi::create_image(const VkExtent2D& extent) const {
    return Image(*device_, device_.allocator(), device_.default_image_format(), extent);
}

ImageView EasyApi::create_image_view(const ImageBase& image) const {
    return ImageView(*device_, *image, image.format(), VK_IMAGE_ASPECT_COLOR_BIT,
                     image.image_type() == VK_IMAGE_TYPE_1D ? VK_IMAGE_VIEW_TYPE_1D
                                                            : VK_IMAGE_VIEW_TYPE_2D);
}

Image EasyApi::create_dashed_line(const glm::vec4 colors[2],
                                  const uint32_t sizes[2]) const {
    std::vector<uint32_t> pixels;
    pixels.reserve(static_cast<size_t>(sizes[0]) + sizes[1]);
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < sizes[i]; j++) {
            uint32_t color = 0;
            for (glm::length_t k = 0; k < 4; k++) {
                color |= static_cast<uint8_t>(colors[i][k] * 255.0) << (k << 3);
            }
            pixels.push_back(color);
        }
    }

    uint32_t width = static_cast<uint32_t>(pixels.size());
    Image result(*device_, device_.allocator(), device_.default_image_format(), width);

    Buffer buffer(device_.allocator(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, pixels);
    CommandBuffer command(*device_, *command_pool_);
    command.begin(true);
    transition_layout(*command, *result, VK_IMAGE_LAYOUT_UNDEFINED,
                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copy_buffer_to_image(*command, *buffer, *result, width, 1);
    transition_layout(*command, *result, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    command.end();

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer = *command;
    submit_info.pCommandBuffers = &command_buffer;

    Fence submit_fence(*device_);
    assert_result(vkQueueSubmit(*transfer_queue_, 1, &submit_info, *submit_fence));

    wait_for_fence(*device_, *submit_fence);

    return result;
}

DescriptorSet EasyApi::allocate_checker_descriptor_set() {
    return descriptor_set_manager_.allocate_descriptor_set(checker_pipeline());
}

FlatFramebuffer EasyApi::create_flat_framebuffer(const ImageBase& image) const {
    return flat_framebuffer_factory_.create_framebuffer(image);
}

RenderPass EasyApi::create_flat_render_pass(VkImageLayout final_layout) const {
    return render_pass_factory_.create_flat_render_pass(final_layout);
}

CommandBuffer EasyApi::create_command_buffer() const {
    return CommandBuffer(*device_, *command_pool_);
}

OverlayRenderer EasyApi::create_overlay_renderer() {
    return OverlayRenderer(device(), overlay_pipeline(), descriptor_set_manager_);
}

CheckerboardRenderer EasyApi::create_checkerboard_renderer() {
    return CheckerboardRenderer(allocator(), checker_pipeline(),
                                descriptor_set_manager_);
}

LineRenderer EasyApi::create_line_renderer() {
    return LineRenderer(allocator(), pipelines_.line_pipeline(),
                        descriptor_set_manager_);
}

GradientRenderer EasyApi::create_gradient_renderer() {
    return GradientRenderer(allocator(), gradient_pipeline(), descriptor_set_manager_);
}

PaletteRenderer EasyApi::create_palette_renderer() {
    return PaletteRenderer(allocator(), palette_pipeline(), descriptor_set_manager_);
}

GfxRenderer EasyApi::create_gfx_renderer() {
    return GfxRenderer(allocator(), pipelines_, descriptor_set_manager_);
}

ObjRenderer EasyApi::create_obj_renderer() {
    return ObjRenderer(allocator(), pipelines_, descriptor_set_manager_);
}

Map16Renderer EasyApi::create_map16_renderer() {
    return Map16Renderer(allocator(), pipelines_, descriptor_set_manager_);
}

ChrRenderer EasyApi::create_chr_renderer() {
    return ChrRenderer(allocator(), pipelines_, descriptor_set_manager_);
}

Surface EasyApi::create_surface(HWND hwnd) const {
    return surface_factory_.create_surface(hwnd);
}

Swapchain EasyApi::create_swapchain(VkSurfaceKHR surface) const {
    return swapchain_factory_.create_swapchain(surface);
}

ManagedSwapchain EasyApi::create_managed_swapchain(VkSurfaceKHR surface) const {
    return swapchain_factory_.create_managed_swapchain(*command_pool_, surface);
}

std::vector<FlatFramebuffer> EasyApi::create_flat_framebuffers(
        const Swapchain& swapchain) const {
    return flat_framebuffer_factory_.create_swapchain_framebuffers(swapchain);
}
}  // namespace maseya::vksnes::win32
