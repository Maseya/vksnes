#include "SnesRenderApi.hxx"

// Local headers
#include <SnesRenderLib/CheckerboardFramebuffer.hxx>
#include <SnesRenderLib/CheckerboardPattern.hxx>
#include <SnesRenderLib/CheckerboardPipeline.hxx>
#include <SnesRenderLib/CheckerboardSurfaceFramebuffers.hxx>
#include <SnesRenderLib/Image.hxx>
#include <SnesRenderLib/Instance.hxx>
#include <SnesRenderLib/PaletteFramebuffer.hxx>
#include <SnesRenderLib/PalettePipeline.hxx>
#include <SnesRenderLib/PaletteSurfaceFramebuffers.hxx>
#include <SnesRenderLib/StbImage.hxx>
#include <SnesRenderLib/Surface.hxx>

using namespace maseya::renderer;

extern "C" {
InstanceHandle __declspec(dllexport) create_instance(
        const char* application_name, uint32_t application_version,
        const VkDebugUtilsMessengerCreateInfoEXT* debug_utils_messenger_create_info) {
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    return new Instance(application_name, application_version,
                        debug_utils_messenger_create_info
                                ? *debug_utils_messenger_create_info
                                : debug_create_info);
}

void __declspec(dllexport) destroy_instance(InstanceHandle instance) {
    delete reinterpret_cast<Instance*>(instance);
}

void __declspec(dllexport) instance_wait_idle(const InstanceHandle instance) {
    reinterpret_cast<const Instance*>(instance)->wait_idle();
}

SurfaceHandle __declspec(dllexport)
        create_win32_surface(const InstanceHandle instance, HWND hwnd) {
    return new Surface(*reinterpret_cast<Instance*>(instance), hwnd);
}

void __declspec(dllexport) destroy_surface(SurfaceHandle surface) {
    delete reinterpret_cast<Surface*>(surface);
}

bool __declspec(dllexport) is_swapchain_valid(const SurfaceHandle surface) {
    return reinterpret_cast<const Surface*>(surface)->is_swapchain_valid();
}

bool __declspec(dllexport)
        present_image_to_surface(SurfaceHandle surface, SwapchainImageHandle image) {
    return reinterpret_cast<Surface*>(surface)->present(
            *reinterpret_cast<SwapchainImage*>(image));
}

bool __declspec(dllexport) try_recreate_swapchain(SurfaceHandle surface) {
    return reinterpret_cast<Surface*>(surface)->try_recreate_swapchain();
}

SwapchainImageHandle __declspec(dllexport)
        acquire_surface_image(SurfaceHandle surface, uint64_t timeout) {
    return reinterpret_cast<Surface*>(surface)->acquire_image(timeout);
}

ImageHandle __declspec(dllexport)
        create_image(const InstanceHandle instance, uint32_t width, uint32_t height,
                     const void* pixels, uint32_t size) {
    if (!pixels || !size) {
        return new Image(*reinterpret_cast<const Instance*>(instance), {width, height});
    } else {
        return new Image(*reinterpret_cast<const Instance*>(instance), {width, height},
                         pixels, size);
    }
}

void __declspec(dllexport) destroy_image(ImageHandle image) {
    delete reinterpret_cast<Image*>(image);
}

void __declspec(dllexport)
        set_image_layout(ImageBaseHandle image, VkImageLayout layout) {
    reinterpret_cast<ImageBase*>(image)->set_layout(layout);
}

CheckerboardPipelineHandle __declspec(dllexport)
        create_checkerboard_pipeline(const InstanceHandle instance) {
    return new CheckerboardPipeline(*reinterpret_cast<const Instance*>(instance));
}

void __declspec(dllexport)
        destroy_checkerboard_pipeline(CheckerboardPipelineHandle pipeline) {
    delete reinterpret_cast<CheckerboardPipeline*>(pipeline);
}

bool __declspec(dllexport)
        wait_framebuffer_idle(const FramebufferBaseHandle framebuffer,
                              uint64_t timeout) {
    return reinterpret_cast<const FramebufferBase*>(framebuffer)->wait_idle(timeout);
}

void __declspec(dllexport)
        set_framebuffer_dependency(FramebufferBaseHandle owner,
                                   FramebufferBaseHandle dependency) {
    reinterpret_cast<FramebufferBase*>(owner)->set_dependency(
            *reinterpret_cast<FramebufferBase*>(dependency));
}

bool __declspec(dllexport)
        framebuffer_has_dependency(const FramebufferBaseHandle framebuffer) {
    return reinterpret_cast<const FramebufferBase*>(framebuffer)->has_dependency();
}

void __declspec(dllexport) render_framebuffer(const FramebufferBaseHandle framebuffer,
                                              uint32_t instance_count) {
    reinterpret_cast<const FramebufferBase*>(framebuffer)->render(instance_count);
}

CheckerboardFramebufferHandle __declspec(dllexport)
        create_checkerboard_framebuffer(const CheckerboardPipelineHandle pipeline,
                                        ImageBaseHandle image,
                                        VkImageLayout final_layout) {
    return new CheckerboardFramebuffer(
            *reinterpret_cast<const CheckerboardPipeline*>(pipeline),
            *reinterpret_cast<ImageBase*>(image), final_layout);
}

void __declspec(dllexport)
        destroy_checkerboard_framebuffer(CheckerboardFramebufferHandle framebuffer) {
    delete reinterpret_cast<CheckerboardFramebuffer*>(framebuffer);
}

void __declspec(dllexport)
        update_checkerboard_pattern(CheckerboardFramebufferHandle framebuffer,
                                    float width, float height, float r1, float g1,
                                    float b1, float a1, float r2, float g2, float b2,
                                    float a2) {
    checkerboard_pattern pattern = {
            {r1, g1, b1, a1},
            {r2, g2, b2, a2},
            {width, height},
    };
    reinterpret_cast<CheckerboardFramebuffer*>(framebuffer)->update_pattern(pattern);
}

PalettePipelineHandle __declspec(dllexport)
        create_palette_pipeline(const InstanceHandle instance) {
    return new PalettePipeline(*reinterpret_cast<const Instance*>(instance));
}

void __declspec(dllexport) destroy_palette_pipeline(PalettePipelineHandle pipeline) {
    delete reinterpret_cast<PalettePipeline*>(pipeline);
}

PaletteFramebufferHandle __declspec(dllexport)
        create_palette_framebuffer(const PalettePipelineHandle pipeline,
                                   ImageBaseHandle image, VkImageLayout final_layout) {
    return new PaletteFramebuffer(*reinterpret_cast<const PalettePipeline*>(pipeline),
                                  *reinterpret_cast<ImageBase*>(image), final_layout);
}

void __declspec(dllexport)
        destroy_palette_framebuffer(PaletteFramebufferHandle framebuffer) {
    delete reinterpret_cast<PaletteFramebuffer*>(framebuffer);
}

void __declspec(dllexport)
        update_palette_color_data(const PaletteFramebufferHandle framebuffer,
                                  const uint16_t* color_data, uint32_t size) {
    reinterpret_cast<const PaletteFramebuffer*>(framebuffer)
            ->update_color_data(color_data, size);
}

void __declspec(dllexport)
        update_palette_view(const PaletteFramebufferHandle framebuffer, uint32_t width,
                            uint32_t height) {
    VkExtent2D size = {width, height};
    reinterpret_cast<const PaletteFramebuffer*>(framebuffer)->update_view(size);
}

FramebufferBaseHandle __declspec(dllexport) get_surface_framebuffer_from_index(
        SurfaceFramebuffersHandle surface_framebuffers, uint32_t index) {
    return &(*reinterpret_cast<SurfaceFramebuffers*>(surface_framebuffers))[index];
}

FramebufferBaseHandle __declspec(dllexport) get_surface_framebuffer_from_image(
        SurfaceFramebuffersHandle surface_framebuffers, SwapchainImageHandle image) {
    return &reinterpret_cast<SurfaceFramebuffers*>(surface_framebuffers)
                    ->get_framebuffer(*reinterpret_cast<SwapchainImage*>(image));
}

uint32_t __declspec(dllexport) get_surface_framebuffers_size(
        const SurfaceFramebuffersHandle surface_framebuffers) {
    return reinterpret_cast<const SurfaceFramebuffers*>(surface_framebuffers)->size();
}

CheckerboardSurfaceFramebuffersHandle __declspec(dllexport)
        create_checkerboard_surface_framebuffers(
                const CheckerboardPipelineHandle pipeline, SurfaceHandle surface,
                VkImageLayout final_layout) {
    return new CheckerboardSurfaceFramebuffers(
            *reinterpret_cast<const CheckerboardPipeline*>(pipeline),
            *reinterpret_cast<Surface*>(surface), final_layout);
}

void __declspec(dllexport) destroy_checkerboard_surface_framebuffers(
        CheckerboardSurfaceFramebuffersHandle surface_framebuffers) {
    delete reinterpret_cast<CheckerboardSurfaceFramebuffers*>(surface_framebuffers);
}

PaletteSurfaceFramebuffersHandle __declspec(dllexport)
        create_palette_surface_framebuffers(const PalettePipelineHandle pipeline,
                                            SurfaceHandle surface,
                                            VkImageLayout final_layout) {
    return new PaletteSurfaceFramebuffers(
            *reinterpret_cast<const PalettePipeline*>(pipeline),
            *reinterpret_cast<Surface*>(surface), final_layout);
}

void __declspec(dllexport) destroy_palette_surface_framebuffers(
        PaletteSurfaceFramebuffersHandle surface_framebuffers) {
    delete reinterpret_cast<PaletteSurfaceFramebuffers*>(surface_framebuffers);
}
}  // extern "C"