#include "print_debug_message.hxx"

// Standard library
#include <iostream>

constexpr bool verbose_output = false;

// Control commands to set console font color/
constexpr char default_font[] = "\033[0m";
constexpr char bold[] = "\033[1m";
constexpr char bold_yellow[] = "\033[1;33m";
constexpr char bold_red[] = "\033[1;31m";

VkBool32 print_debug_message(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT& callback_data) {
    if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::cout << bold_red << callback_data.pMessage << default_font << std::endl
                  << std::endl;
    } else if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cout << bold_yellow << callback_data.pMessage << default_font << std::endl
                  << std::endl;
    } else if constexpr (verbose_output) {
        std::cout << bold << callback_data.pMessage << default_font << std::endl
                  << std::endl;
    }

    return false;
}