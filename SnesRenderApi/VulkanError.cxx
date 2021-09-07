#include "VulkanError.hxx"

namespace maseya::renderer {
namespace {
std::string get_last_win_api_error(DWORD code);
}

VulkanError::VulkanError(const std::string& message)
        : std::runtime_error(message.c_str()) {}

VulkanWinApiError::VulkanWinApiError() : VulkanWinApiError(GetLastError()) {
    SetLastError(0);
}
VulkanWinApiError::VulkanWinApiError(DWORD code)
        : VulkanWinApiError(code, get_last_win_api_error(code)) {}
VulkanWinApiError::VulkanWinApiError(DWORD code, const std::string& message)
        : VulkanError(message), code_(code) {}

namespace {
std::string get_last_win_api_error(DWORD code) {
    constexpr static DWORD format_flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                          FORMAT_MESSAGE_FROM_SYSTEM |
                                          FORMAT_MESSAGE_IGNORE_INSERTS;
    LPSTR buffer{};
    if (FormatMessageA(format_flags, NULL, code, 0, reinterpret_cast<LPSTR>(&buffer), 0,
                       NULL) == 0) {
        throw VulkanWinApiError();
    }

    std::string result(buffer);
    LocalFree(buffer);
    return result;
}
}  // namespace
}  // namespace maseya::renderer