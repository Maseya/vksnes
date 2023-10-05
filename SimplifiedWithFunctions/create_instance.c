#include "create_instance.h"

// Local headers
#include "report.h"

VkResult create_instance(VkInstance* instance) {
    VkApplicationInfo app_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .applicationVersion = "SNES Dirty API With functions",
            .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
            .pEngineName = "No engine",
            .engineVersion = VK_MAKE_VERSION(0, 0, 0),
            .apiVersion = VK_API_VERSION_1_0,
    };

    VkInstanceCreateInfo instance_create_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &app_info,
            .enabledLayerCount = 1,
            .ppEnabledLayerNames = layers,
            .enabledExtensionCount = 1,
            .ppEnabledExtensionNames = extensions,
    };

    return vkCreateInstance(&instance_create_info, &callbacks, instance);
}