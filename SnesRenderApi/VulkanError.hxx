#pragma once

// External APIs
#include <Windows.h>

// Standard library
#include <stdexcept>
#include <string>

namespace maseya::renderer {
class VulkanError : public std::runtime_error {
public:
    VulkanError(const std::string& message);
};

class VulkanWinApiError : public VulkanError {
public:
    VulkanWinApiError();
    VulkanWinApiError(DWORD error_code);
    VulkanWinApiError(DWORD error_code, const std::string& message);

    DWORD code() const { return code_; }

private:
    DWORD code_;
};
}  // namespace maseya::renderer
