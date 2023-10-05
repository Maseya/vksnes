#pragma once

// External APIs
#include <Windows.h>
#include <vulkan/vulkan.h>

// Standard library
#include <cstdint>

typedef void* CheckerboardFramebufferHandle;
typedef void* CheckerboardPipelineHandle;
typedef void* CheckerboardSurfaceFramebuffersHandle;
typedef void* FramebufferBaseHandle;
typedef void* ImageHandle;
typedef void* ImageBaseHandle;
typedef void* InstanceHandle;
typedef void* PaletteFramebufferHandle;
typedef void* PalettePipelineHandle;
typedef void* PaletteSurfaceFramebuffersHandle;
typedef void* SurfaceHandle;
typedef void* SurfaceFramebuffersHandle;
typedef void* SwapchainImageHandle;

extern "C" {
InstanceHandle __declspec(dllexport) create_instance(
        const char* application_name, uint32_t application_version,
        const VkDebugUtilsMessengerCreateInfoEXT* debug_utils_messenger_create_info);

void __declspec(dllexport) destroy_instance(InstanceHandle instance);

void __declspec(dllexport) instance_wait_idle(const InstanceHandle instance);

SurfaceHandle __declspec(dllexport)
        create_win32_surface(const InstanceHandle instance, HWND hwnd);

void __declspec(dllexport) destroy_surface(SurfaceHandle surface);

bool __declspec(dllexport) is_swapchain_valid(const SurfaceHandle surface);

SwapchainImageHandle __declspec(dllexport)
        acquire_surface_image(SurfaceHandle surface, uint64_t timeout);

bool __declspec(dllexport)
        present_image_to_surface(SurfaceHandle surface, SwapchainImageHandle image);

bool __declspec(dllexport) try_recreate_swapchain(SurfaceHandle surface);

ImageHandle __declspec(dllexport)
        create_image(const InstanceHandle instance, uint32_t width, uint32_t height,
                     const void* pixels, uint32_t size);

void __declspec(dllexport) destroy_image(ImageHandle image);

void __declspec(dllexport)
        set_image_layout(ImageBaseHandle image, VkImageLayout layout);

CheckerboardPipelineHandle __declspec(dllexport)
        create_checkerboard_pipeline(const InstanceHandle instance);

void __declspec(dllexport)
        destroy_checkerboard_pipeline(CheckerboardPipelineHandle pipeline);

PalettePipelineHandle __declspec(dllexport)
        create_palette_pipeline(const InstanceHandle instance);

void __declspec(dllexport) destroy_palette_pipeline(PalettePipelineHandle pipeline);

bool __declspec(dllexport)
        wait_framebuffer_idle(const FramebufferBaseHandle framebuffer,
                              uint64_t timeout);

void __declspec(dllexport) set_framebuffer_dependency(FramebufferBaseHandle owner,
                                                      FramebufferBaseHandle dependency);

bool __declspec(dllexport)
        framebuffer_has_dependency(const FramebufferBaseHandle framebuffer);

void __declspec(dllexport) render_framebuffer(const FramebufferBaseHandle framebuffer,
                                              uint32_t instance_count);

CheckerboardFramebufferHandle __declspec(dllexport)
        create_checkerboard_framebuffer(const CheckerboardPipelineHandle pipeline,
                                        ImageBaseHandle image,
                                        VkImageLayout final_layout);

void __declspec(dllexport)
        destroy_checkerboard_framebuffer(CheckerboardFramebufferHandle framebuffer);

void __declspec(dllexport)
        update_checkerboard_pattern(CheckerboardFramebufferHandle framebuffer,
                                    float width, float height, float a1, float r1,
                                    float g1, float b1, float a2, float r2, float g2,
                                    float b2);

PaletteFramebufferHandle __declspec(dllexport)
        create_palette_framebuffer(const PalettePipelineHandle pipeline,
                                   ImageBaseHandle image, VkImageLayout final_layout);

void __declspec(dllexport)
        destroy_palette_framebuffer(PaletteFramebufferHandle framebuffer);

void __declspec(dllexport)
        update_palette_color_data(const PaletteFramebufferHandle framebuffer,
                                  const uint16_t* color_data, uint32_t size);

void __declspec(dllexport)
        update_palette_view(const PaletteFramebufferHandle framebuffer, uint32_t width,
                            uint32_t height);

FramebufferBaseHandle __declspec(dllexport) get_surface_framebuffer_from_index(
        SurfaceFramebuffersHandle surface_framebuffers, uint32_t index);

FramebufferBaseHandle __declspec(dllexport) get_surface_framebuffer_from_image(
        SurfaceFramebuffersHandle surface_framebuffers, SwapchainImageHandle image);

uint32_t __declspec(dllexport) get_surface_framebuffers_size(
        const SurfaceFramebuffersHandle surface_framebuffers);

CheckerboardSurfaceFramebuffersHandle __declspec(dllexport)
        create_checkerboard_surface_framebuffers(
                const CheckerboardPipelineHandle pipeline, SurfaceHandle surface,
                VkImageLayout final_layout);

void __declspec(dllexport) destroy_checkerboard_surface_framebuffers(
        CheckerboardSurfaceFramebuffersHandle surface_framebuffers);

PaletteSurfaceFramebuffersHandle __declspec(dllexport)
        create_palette_surface_framebuffers(const PalettePipelineHandle pipeline,
                                            SurfaceHandle surface,
                                            VkImageLayout final_layout);

void __declspec(dllexport) destroy_palette_surface_framebuffers(
        PaletteSurfaceFramebuffersHandle surface_framebuffers);
}  // extern "C"