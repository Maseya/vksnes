#pragma once

// External APIs
#include <vulkan/vulkan.h>

VkBool32 print_debug_message(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                             VkDebugUtilsMessageTypeFlagsEXT message_type,
                             const VkDebugUtilsMessengerCallbackDataEXT& callback_data);