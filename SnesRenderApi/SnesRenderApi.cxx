#include "SnesRenderApi.hxx"

extern "C" {
InstanceHandle DLLEXPORT create_instance(
        const char* application_name,
        const maseya::renderer::Version* application_version,
        const vk::DebugUtilsMessengerCreateInfoEXT* debug_utils_messenger_create_info) {
    InstanceHandle result = new maseya::renderer::Instance(
            application_name,
            application_version ? *application_version : maseya::renderer::Version(1),
            debug_utils_messenger_create_info ? *debug_utils_messenger_create_info
                                              : vk::DebugUtilsMessengerCreateInfoEXT());

    // TODO(nrg): Deal with errors.

    return result;
}

void DLLEXPORT destroy_instance(InstanceHandle handle) { delete handle; }

SurfaceHandle DLLEXPORT create_win32_surface(const InstanceHandle instance, HWND hwnd) {
    SurfaceHandle result = new maseya::renderer::Surface(*instance, hwnd);

    return result;
}

void DLLEXPORT destroy_surface(SurfaceHandle surface) { delete surface; }

RenderTargetHandle DLLEXPORT acquire_render_target_from_surface(SurfaceHandle surface,
                                                                uint32_t timeout) {
    maseya::renderer::RenderTarget result = surface->acquire_render_target(timeout);
    if (!result) {
        return nullptr;
    }

    return new maseya::renderer::RenderTarget(result);
}

void DLLEXPORT destroy_render_target(RenderTargetHandle render_target) {
    delete render_target;
}

bool DLLEXPORT present_render_target_to_surface(SurfaceHandle surface,
                                                RenderTargetHandle render_target) {
    return surface->present(*render_target);
}

void DLLEXPORT render_test_image(const InstanceHandle instance,
                                 RenderTargetHandle render_target) {
    instance->render_test_image(*render_target);
}

TestVertexBufferHandle DLLEXPORT
create_test_vertex_buffers(const SurfaceHandle surface) {
    return new maseya::renderer::VertexBufferHelper(
            surface->create_vertex_buffer_helper());
}

bool DLLEXPORT update_vertex_buffers(TestVertexBufferHandle vertex_buffers,
                                     const void* data, size_t vertex_count,
                                     size_t vertex_size, uint64_t timeout) {
    return vertex_buffers->update_buffers(data, vertex_count, vertex_size, timeout);
}

void DLLEXPORT destroy_test_vertex_buffers(TestVertexBufferHandle vertex_buffers) {
    delete vertex_buffers;
}

TestUniformBuffersHandle DLLEXPORT
create_test_uniform_buffers(const SurfaceHandle surface) {
    return new maseya::renderer::TestUniformBuffers(
            surface->create_test_surface_buffers());
}

void DLLEXPORT destroy_test_uniform_buffers(TestUniformBuffersHandle uniform_buffers) {
    delete uniform_buffers;
}

void DLLEXPORT render_test_image_with_buffers(
        const InstanceHandle instance, RenderTargetHandle render_target,
        const TestUniformBuffersHandle uniform_buffers,
        const TestVertexBufferHandle vertex_buffers, float angle) {
    instance->render_test_image_with_buffers(*render_target, *uniform_buffers,
                                             *vertex_buffers, angle);
}
}  // extern "C"