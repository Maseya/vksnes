#pragma once

// Local headers
#include "PalettePipeline.hxx"
#include "QueueFamilyIndices.hxx"
#include "RenderTarget.hxx"
#include "TestPipeline.hxx"
#include "TestUniformBufferPipeline.hxx"
#include "TestUniformBuffers.hxx"
#include "Version.hxx"
#include "Vertex.hxx"
#include "vma_unique_handles.hxx"

// External APIs
#include <vulkan/vulkan.hpp>

// Standard library
#include <cstdint>
#include <string>

namespace maseya::renderer {
template <class UserCallback>
VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
               VkDebugUtilsMessageTypeFlagsEXT message_type,
               const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* userp);

template <class UserCallback>
vk::DebugUtilsMessengerCreateInfoEXT create_debug_messenger_info(
        UserCallback& user_callback_);

class Instance {
private:
    constexpr static char default_application_name[] = "";
    constexpr static Version default_application_version = Version(1);

    constexpr static uint64_t no_timeout = UINT64_MAX;

public:
    template <class UserCallback>
    Instance(UserCallback& debug_callback)
            : Instance(default_application_name, debug_callback) {}
    template <class UserCallback>
    Instance(const std::string& application_name, UserCallback& debug_callback)
            : Instance(application_name, default_application_version, debug_callback) {}
    template <class UserCallback>
    Instance(const std::string& application_name, const Version& version,
             UserCallback& debug_callback)
            : Instance(application_name, version,
                       create_debug_messenger_info(debug_callback)) {}

    Instance(const char* application_name) : Instance(std::string(application_name)) {}
    Instance(const std::string& application_name,
             const vk::DebugUtilsMessengerCreateInfoEXT&
                     debug_utils_messenger_create_info)
            : Instance(application_name, default_application_version,
                       debug_utils_messenger_create_info) {}
    Instance(const vk::DebugUtilsMessengerCreateInfoEXT&
                     debug_utils_messenger_create_info)
            : Instance(default_application_name, default_application_version,
                       debug_utils_messenger_create_info) {}
    Instance(const std::string& application_name = default_application_name,
             const Version& application_version = default_application_version,
             const vk::DebugUtilsMessengerCreateInfoEXT&
                     debug_utils_messenger_create_info = {});

    Instance(const Instance&) = delete;
    Instance(Instance&&) noexcept = default;

    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&) noexcept = default;

    ~Instance() {
        // Let any final commands finish before we start destroying stuff.
        device_->waitIdle();
    }

    const std::string& application_name() const { return application_name_; }
    const Version& application_version() const { return application_version_; }

    void render_test_image(RenderTarget& render_target) const {
        test_pipeline_.render(render_target);
    }

    void render_test_image_with_buffers(RenderTarget& render_target,
                                        const TestUniformBuffers& uniform_buffer,
                                        const VertexBufferHelper& vertex_buffer,
                                        float angle) {
        test_uniform_buffer_pipeline_.render(render_target, uniform_buffer,
                                             vertex_buffer, angle);
    }

    bool wait_for_fence(vk::Fence fence, uint64_t timeout = no_timeout) const;

    bool is_fence_signaled(vk::Fence fence) const { return wait_for_fence(fence, 0); }

private:
    const vk::Instance& instance() const { return *instance_; }
    const vk::DispatchLoaderDynamic& dispatch_loader_dynamic() const {
        return dispatch_loader_dynamic_;
    }
    const vk::PhysicalDevice& physical_device() const { return physical_device_; }
    const vk::Format& default_format() const { return default_format_; }
    const uint32_t& graphics_queue_index() const {
        return queue_family_indices_.graphics_family();
    }
    const uint32_t& presentation_queue_index() const {
        return queue_family_indices_.presentation_family();
    }
    const vk::Device& device() const { return *device_; }
    const vk::Queue& graphics_queue() const { return graphics_queue_; }
    const vk::Queue& presentation_queue() const { return presentation_queue_; }
    const vk::CommandPool& command_pool() const { return *command_pool_; }
    const vma::Allocator& allocator() const { return *allocator_; }

    const TestPipeline& test_pipeline() const { return test_pipeline_; }
    const TestUniformBufferPipeline& test_uniform_buffer_pipeline() const {
        return test_uniform_buffer_pipeline_;
    }
    const PalettePipeline& palette_pipeline() const { return palette_pipeline_; }

private:
    std::string application_name_;
    Version application_version_;

    vk::UniqueInstance instance_;
    vk::DispatchLoaderDynamic dispatch_loader_dynamic_;
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>
            debug_utils_messenger_;
    vk::PhysicalDevice physical_device_;
    vk::Format default_format_;
    QueueFamilyIndices queue_family_indices_;
    vk::UniqueDevice device_;
    vk::Queue graphics_queue_;
    vk::Queue presentation_queue_;
    vk::UniqueCommandPool command_pool_;
    vma::UniqueAllocator allocator_;

    TestPipeline test_pipeline_;
    TestUniformBufferPipeline test_uniform_buffer_pipeline_;
    PalettePipeline palette_pipeline_;

    friend class Surface;
    friend class TestPipeline;
    friend class TestUniformBufferPipeline;
    friend class TestUniformBuffers;
    friend class PalettePipeline;
};

template <class UserCallback>
VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
               VkDebugUtilsMessageTypeFlagsEXT message_type,
               const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* userp) {
    return (*reinterpret_cast<UserCallback*>(userp))(
            vk::DebugUtilsMessageSeverityFlagBitsEXT(message_severity),
            vk::DebugUtilsMessageTypeFlagBitsEXT(message_type),
            *reinterpret_cast<const vk::DebugUtilsMessengerCallbackDataEXT*>(
                    callback_data));
}

template <class UserCallback>
vk::DebugUtilsMessengerCreateInfoEXT create_debug_messenger_info(
        UserCallback& user_callback_) {
    constexpr auto severity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                              vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                              vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    constexpr auto type = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                          vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                          vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

    return vk::DebugUtilsMessengerCreateInfoEXT(
            {}, severity, type, debug_callback<UserCallback>, &user_callback_);
}
}  // namespace maseya::renderer