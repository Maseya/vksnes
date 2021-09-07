#include "Instance.hxx"

// Local headers
#include "SwapChainSupportDetails.hxx"
#include "VulkanError.hxx"
#include "create_buffer.hxx"
#include "get_default_format.hxx"

// External APIs
#include <Windows.h>

// Standard library
#include <algorithm>
#include <cstring>
#include <iostream>
#include <unordered_set>
#include <vector>

namespace maseya::renderer {
namespace {
constexpr static char engine_name[] = "Super NES Graphics Rendering API";
constexpr static Version engine_version = Version(1);
constexpr static uint32_t vulkan_api_version = VK_API_VERSION_1_0;

#if defined(DEBUG) || defined(_DEBUG)
constexpr bool enable_validation_layers = true;
#else
constexpr bool enable_validation_layers = false;
#endif

constexpr const char* validation_layers[] = {"VK_LAYER_KHRONOS_validation"};

constexpr const char* required_instance_extensions[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
#if defined(DEBUG) || defined(_DEBUG)
        "VK_EXT_debug_utils",
#endif
};
constexpr const char* required_physical_device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

vk::UniqueInstance create_instance(
        const std::string& application_name, const Version& application_version,
        const vk::DebugUtilsMessengerCreateInfoEXT& debug_messenger_create_info);

vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>
create_debug_messenger(
        vk::Instance instance,
        const vk::DebugUtilsMessengerCreateInfoEXT& debug_messenger_create_info,
        const vk::DispatchLoaderDynamic& dispatch_loader_dynamic);

vk::PhysicalDevice pick_physical_device(vk::Instance instace);

int score_physical_device(vk::Instance instance, vk::PhysicalDevice physical_device);

// Define ordering of Physical Devices based on suitability of API.
struct PhysicalDeviceMetric {
    PhysicalDeviceMetric(vk::Instance instance, vk::PhysicalDevice physical_device)
            : physical_device(physical_device),
              score(score_physical_device(instance, physical_device)) {}

    bool operator<(const PhysicalDeviceMetric& rhs) const { return score < rhs.score; }

    vk::PhysicalDevice physical_device;
    int score;
};

vk::UniqueDevice create_logical_device(
        vk::PhysicalDevice physical_device,
        const std::unordered_set<uint32_t>& unique_queue_families);

vk::UniqueCommandPool create_command_pool(vk::Device device, uint32_t graphics_family);

vma::UniqueAllocator create_vma_allocator(
        vk::Instance instance, vk::PhysicalDevice physical_device, vk::Device device,
        vk::Optional<const vk::AllocationCallbacks> allocation_callbacks = nullptr);
}  // namespace

Instance::Instance(
        const std::string& application_name, const Version& application_version,
        const vk::DebugUtilsMessengerCreateInfoEXT& debug_messenger_create_info)
        : application_name_(application_name),
          application_version_(application_version),
          instance_(create_instance(application_name, application_version,
                                    debug_messenger_create_info)),
          dispatch_loader_dynamic_(*instance_, ::vkGetInstanceProcAddr),
          debug_utils_messenger_(create_debug_messenger(
                  *instance_, debug_messenger_create_info, dispatch_loader_dynamic_)),
          physical_device_(pick_physical_device(*instance_)),
          default_format_(get_default_format(*instance_, physical_device_)),
          queue_family_indices_(physical_device_),
          device_(create_logical_device(physical_device_,
                                        queue_family_indices_.unique_queue_families())),
          graphics_queue_(
                  device_->getQueue(queue_family_indices_.graphics_family(), 0)),
          presentation_queue_(
                  device_->getQueue(queue_family_indices_.presentation_family(), 0)),
          command_pool_(create_command_pool(*device_,
                                            queue_family_indices_.graphics_family())),
          allocator_(create_vma_allocator(*instance_, physical_device_, *device_)),
          test_pipeline_(device(), default_format()),
          test_uniform_buffer_pipeline_(device(), default_format()),
          palette_pipeline_(device(), default_format()) {}

bool Instance::wait_for_fence(vk::Fence fence, uint64_t timeout) const {
    vk::Result result = device().waitForFences(1, &fence, true, no_timeout);
    switch (result) {
        case vk::Result::eErrorOutOfHostMemory:
            throw VulkanError("Ran out of host memory while waiting for fence.");
        case vk::Result::eErrorOutOfDeviceMemory:
            throw VulkanError("Ran out of device memory while waiting for fence.");
        case vk::Result::eErrorDeviceLost:
            throw VulkanError("Lost device!");
    }

    return result == vk::Result::eSuccess;
}

namespace {
bool all_required_extensions_supported();

bool validation_layers_supported();

vk::UniqueInstance create_instance(
        const std::string& application_name, const Version& application_version,
        const vk::DebugUtilsMessengerCreateInfoEXT& debug_messenger_create_info) {
    if (!all_required_extensions_supported()) {
        throw VulkanError(
                "Required extensions were not available in this Vulkan driver.");
    }

#pragma warning(push)
#pragma warning(disable : 6237)
    if (enable_validation_layers && !validation_layers_supported()) {
        throw VulkanError("Validation layers requested, but not available.");
    }
#pragma warning(pop)

    vk::ApplicationInfo app_info;
    app_info.pApplicationName = application_name.c_str();
    app_info.applicationVersion = application_version.make_version();
    app_info.pEngineName = engine_name;
    app_info.engineVersion = engine_version.make_version();
    app_info.apiVersion = vulkan_api_version;

    vk::InstanceCreateInfo create_info;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount =
            static_cast<uint32_t>(std::size(required_instance_extensions));
    create_info.ppEnabledExtensionNames = required_instance_extensions;

    if (enable_validation_layers) {
        create_info.pNext = &debug_messenger_create_info;
        create_info.enabledLayerCount =
                static_cast<uint32_t>(std::size(validation_layers));
        create_info.ppEnabledLayerNames = validation_layers;
    }

    return vk::createInstanceUnique(create_info);
}

vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>
create_debug_messenger(
        vk::Instance instance,
        const vk::DebugUtilsMessengerCreateInfoEXT& debug_messenger_create_info,
        const vk::DispatchLoaderDynamic& dispatch_loader_dynamic) {
    if (enable_validation_layers) {
        return instance.createDebugUtilsMessengerEXTUnique(
                debug_messenger_create_info, nullptr, dispatch_loader_dynamic);
    }
    return vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>();
}

bool all_required_extensions_supported() {
    std::vector<vk::ExtensionProperties> supported_extensions(
            vk::enumerateInstanceExtensionProperties());

    auto is_extension_supported = [&supported_extensions](const char* name) -> bool {
        auto match_extension_name = [name](vk::ExtensionProperties extension) -> bool {
            return std::strcmp(extension.extensionName, name) == 0;
        };
        return std::any_of(supported_extensions.cbegin(), supported_extensions.cend(),
                           match_extension_name);
    };

    return std::all_of(std::cbegin(required_instance_extensions),
                       std::cend(required_instance_extensions), is_extension_supported);
}

bool validation_layers_supported() {
    std::vector<vk::LayerProperties> layer_properties =
            vk::enumerateInstanceLayerProperties();

    auto is_layer_available = [&layer_properties](const char* name) {
        auto match_layer_name = [name](const vk::LayerProperties& layer_property) {
            return std::strcmp(name, layer_property.layerName) == 0;
        };
        return std::any_of(layer_properties.cbegin(), layer_properties.cend(),
                           match_layer_name);
    };

    return std::all_of(std::cbegin(validation_layers), std::cend(validation_layers),
                       is_layer_available);
}

vk::PhysicalDevice pick_physical_device(vk::Instance instance) {
    std::vector<vk::PhysicalDevice> physical_devices =
            instance.enumeratePhysicalDevices();
    if (physical_devices.empty()) {
        throw VulkanError("Failed to find any GPU with Vulkan support.");
    }

    std::vector<PhysicalDeviceMetric> physical_device_scores;
    for (vk::PhysicalDevice physical_device : physical_devices) {
        physical_device_scores.emplace_back(instance, physical_device);
    }

    auto best = std::max_element(physical_device_scores.begin(),
                                 physical_device_scores.end());
    if (best->score <= 0) {
        throw VulkanError("Failed to find a GPU meeting minimum requirements.");
    }
    return best->physical_device;
}

bool check_physical_device_supports_required_extensions(
        vk::PhysicalDevice physical_device);

int score_physical_device(vk::Instance instance, vk::PhysicalDevice physical_device) {
    QueueFamilyIndices indices(physical_device);
    if (!indices) {
        return 0;
    }

    if (!check_physical_device_supports_required_extensions(physical_device)) {
        return 0;
    }

    vk::PhysicalDeviceFeatures features = physical_device.getFeatures();
    if (!features.geometryShader) {
        return 0;
    }

    // Any physical devices so far are suitable enough for minimum use.
    int result = 1;

    vk::PhysicalDeviceProperties properties = physical_device.getProperties();
    if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        result += 1000;
    }
    if (get_default_format(instance, physical_device) == vk::Format::eB8G8R8A8Srgb) {
        result += 10;
    }

    return result;
}

bool check_physical_device_supports_required_extensions(vk::PhysicalDevice device) {
    std::vector<vk::ExtensionProperties> supported_extensions =
            device.enumerateDeviceExtensionProperties();

    auto extension_is_supported = [&supported_extensions](const char* name) -> bool {
        auto match_extension_name = [name](vk::ExtensionProperties extension) -> bool {
            return std::strcmp(extension.extensionName, name) == 0;
        };
        return std::any_of(supported_extensions.cbegin(), supported_extensions.cend(),
                           match_extension_name);
    };

    return std::all_of(std::cbegin(required_physical_device_extensions),
                       std::cend(required_physical_device_extensions),
                       extension_is_supported);
}

vk::UniqueDevice create_logical_device(
        vk::PhysicalDevice physical_device,
        const std::unordered_set<uint32_t>& unique_queue_families) {
    float queue_priority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    for (uint32_t queue_family : unique_queue_families) {
        vk::DeviceQueueCreateInfo queue_create_info;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    vk::PhysicalDeviceFeatures physical_device_features;

    vk::DeviceCreateInfo device_create_info;
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
    device_create_info.queueCreateInfoCount =
            static_cast<uint32_t>(queue_create_infos.size());
    device_create_info.pEnabledFeatures = &physical_device_features;
    device_create_info.enabledExtensionCount =
            static_cast<uint32_t>(std::size(required_physical_device_extensions));
    device_create_info.ppEnabledExtensionNames = required_physical_device_extensions;
    if (enable_validation_layers) {
        device_create_info.enabledLayerCount =
                static_cast<uint32_t>(std::size(validation_layers));
        device_create_info.ppEnabledLayerNames = validation_layers;
    }

    return physical_device.createDeviceUnique(device_create_info);
}

vk::UniqueCommandPool create_command_pool(vk::Device device, uint32_t graphics_family) {
    vk::CommandPoolCreateInfo create_info;
    create_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    create_info.queueFamilyIndex = graphics_family;
    return device.createCommandPoolUnique(create_info);
}

vma::UniqueAllocator create_vma_allocator(
        vk::Instance instance, vk::PhysicalDevice physical_device, vk::Device device,
        vk::Optional<const vk::AllocationCallbacks> allocation_callbacks) {
    vma::AllocatorCreateInfo create_info;
    create_info.vulkanApiVersion = VK_API_VERSION_1_0;
    create_info.instance = instance;
    create_info.physicalDevice = physical_device;
    create_info.device = device;
    create_info.pAllocationCallbacks =
            static_cast<const vk::AllocationCallbacks*>(allocation_callbacks);

    vma::Allocator allocator;
    vk::Result result = vma::createAllocator(&create_info, &allocator);
    if (result != vk::Result::eSuccess) {
        throw VulkanError("Could not create vmaAllocator");
    }

    return vma::UniqueAllocator(allocator);
}
}  // namespace
}  // namespace maseya::renderer