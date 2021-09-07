#pragma once

// Local headers
#include "Instance.hxx"
#include "RenderTarget.hxx"
#include "Surface.hxx"
#include "TestUniformBuffers.hxx"
#include "Version.hxx"
#include "VertexBufferHelper.hxx"

#define DLLEXPORT __declspec(dllexport)

extern "C" {
typedef maseya::renderer::Instance* InstanceHandle;
typedef maseya::renderer::Surface* SurfaceHandle;
typedef maseya::renderer::RenderTarget* RenderTargetHandle;
typedef maseya::renderer::TestUniformBuffers* TestUniformBuffersHandle;
typedef maseya::renderer::VertexBufferHelper* TestVertexBufferHandle;

InstanceHandle DLLEXPORT create_instance(
        const char* application_name,
        const maseya::renderer::Version* application_version,
        const vk::DebugUtilsMessengerCreateInfoEXT* debug_utils_messenger_create_info);

void DLLEXPORT destroy_instance(InstanceHandle handle);

SurfaceHandle DLLEXPORT create_win32_surface(const InstanceHandle instance, HWND hwnd);

void DLLEXPORT destroy_surface(SurfaceHandle surface);

RenderTargetHandle DLLEXPORT acquire_render_target_from_surface(SurfaceHandle surface,
                                                                uint32_t timeout);

void DLLEXPORT destroy_render_target(RenderTargetHandle render_target);

bool DLLEXPORT present_render_target_to_surface(SurfaceHandle surface,
                                                const RenderTargetHandle render_target);

void DLLEXPORT render_test_image(const InstanceHandle instance,
                                 RenderTargetHandle render_target);

TestVertexBufferHandle DLLEXPORT
create_test_vertex_buffers(const SurfaceHandle surface);

bool DLLEXPORT update_vertex_buffers(TestVertexBufferHandle vertex_buffers,
                                     const void* data, size_t vertex_count,
                                     size_t vertex_size, uint64_t timeout);

void DLLEXPORT destroy_test_vertex_buffers(TestVertexBufferHandle vertex_buffers);

TestUniformBuffersHandle DLLEXPORT
create_test_uniform_buffers(const SurfaceHandle surface);

void DLLEXPORT destroy_test_uniform_buffers(TestUniformBuffersHandle uniform_buffers);

void DLLEXPORT render_test_image_with_buffers(
        const InstanceHandle instance, RenderTargetHandle render_target,
        const TestUniformBuffersHandle uniform_buffers,
        const TestVertexBufferHandle vertex_buffers, float angle);
}  // extern "C"