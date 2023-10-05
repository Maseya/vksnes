#ifndef CREATE_INSTANCE_H
#define CREATE_INSTANCE_H

// External APIs
#include <vulkan/vulkan.h>

static const char* extensions[] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
static const char* layers[] = {"VK_LAYER_KHRONOS_validation"};

VkResult create_instance(VkInstance* instance);

#endif