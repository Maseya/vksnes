#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

// Standard library
#include <vector>

namespace maseya::renderer {
class SwapChainSupportDetails {
public:
    SwapChainSupportDetails(vk::PhysicalDevice device, vk::SurfaceKHR surface,
                            const vk::Extent2D& desired_extent = {});

    const std::vector<vk::SurfaceFormatKHR> surface_formats() const {
        return surface_formats_;
    }
    const std::vector<vk::PresentModeKHR> present_modes() const {
        return present_modes_;
    }
    const vk::SurfaceCapabilitiesKHR& capabilities() const { return capabilities_; }

    operator bool() const {
        return !surface_formats_.empty() && !present_modes_.empty();
    }
    bool operator!() const { return !static_cast<bool>(*this); }

    const vk::SurfaceFormatKHR& surface_format() const { return *surface_format_; }
    vk::Format format() const { return surface_format_->format; }
    vk::PresentModeKHR present_mode() const { return present_mode_; }
    vk::Extent2D extent() const { return extent_; }
    vk::SwapchainCreateInfoKHR swap_chain_create_info(
            vk::SwapchainKHR old_swapchain = nullptr) const;

private:
    const vk::SurfaceFormatKHR* construct_surface_format() const;
    vk::PresentModeKHR construct_present_mode() const;
    vk::Extent2D construct_extent(const vk::Extent2D& desired) const;

private:
    vk::PhysicalDevice physical_device_;
    vk::SurfaceKHR surface_;
    vk::SurfaceCapabilitiesKHR capabilities_;
    std::vector<vk::SurfaceFormatKHR> surface_formats_;
    std::vector<vk::PresentModeKHR> present_modes_;
    const vk::SurfaceFormatKHR* surface_format_;
    vk::PresentModeKHR present_mode_;
    vk::Extent2D extent_;
};
}  // namespace maseya::renderer