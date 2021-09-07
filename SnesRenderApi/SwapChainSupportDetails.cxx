#include "SwapChainSupportDetails.hxx"

// Local headers
#include "QueueFamilyIndices.hxx"

// External APIs
#include <vulkan/vulkan.hpp>

// Standard library
#include <algorithm>
#include <cstdint>

namespace maseya::renderer {
SwapChainSupportDetails::SwapChainSupportDetails(vk::PhysicalDevice physical_device,
                                                 vk::SurfaceKHR surface,
                                                 const vk::Extent2D& desired_extent)
        : physical_device_(physical_device),
          surface_(surface),
          capabilities_(physical_device.getSurfaceCapabilitiesKHR(surface)),
          surface_formats_(physical_device.getSurfaceFormatsKHR(surface)),
          present_modes_(physical_device.getSurfacePresentModesKHR(surface)),
          surface_format_(construct_surface_format()),
          present_mode_(construct_present_mode()),
          extent_(construct_extent(desired_extent)) {}

const vk::SurfaceFormatKHR* SwapChainSupportDetails::construct_surface_format() const {
    if (!*this) {
        return nullptr;
    }

    // We'll use a nonlinear, 32-bit SRGB color space.
    auto is_preferred_format = [](const vk::SurfaceFormatKHR format) {
        return format.format == vk::Format::eB8G8R8A8Srgb &&
               format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
    };
    auto result = std::find_if(surface_formats_.begin(), surface_formats_.end(),
                               is_preferred_format);

    // If we don't have our desired format, we can just default to the first available
    // format.
    return result != surface_formats_.end() ? &(*result) : &surface_formats_[0];
}

vk::PresentModeKHR SwapChainSupportDetails::construct_present_mode() const {
    if (!*this) {
        return vk::PresentModeKHR::eFifo;
    }

    // We'll be looking for triple buffer mode to maintain relatively low latency while
    // still avoiding tearing.
    auto is_preferred_mode = [](vk::PresentModeKHR mode) {
        return mode == vk::PresentModeKHR::eMailbox;
    };
    auto result = std::find_if(present_modes_.begin(), present_modes_.end(),
                               is_preferred_mode);

    // If desired mode not found, then FIFO is always guaranteed to exist, so we fall
    // back to that.
    return result != present_modes_.end() ? *result : vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChainSupportDetails::construct_extent(
        const vk::Extent2D& desired) const {
    if (!*this) {
        return vk::Extent2D();
    }

    // MAX is a special value that tells us any size can be used. If we don't get that
    // value, then we must the original queried extent.
    if (capabilities_.currentExtent.width != UINT32_MAX) {
        return capabilities_.currentExtent;
    }

    // Otherwise, we try to return the desired extent, clamped between the minimum and
    // maximum image extent.
    return vk::Extent2D(std::clamp(desired.width, capabilities_.minImageExtent.width,
                                   capabilities_.maxImageExtent.width),
                        std::clamp(desired.height, capabilities_.minImageExtent.height,
                                   capabilities_.maxImageExtent.height));
}

vk::SwapchainCreateInfoKHR SwapChainSupportDetails::swap_chain_create_info(
        vk::SwapchainKHR old_swapchain) const {
    if (!*this) {
        return vk::SwapchainCreateInfoKHR();
    }

    // Try to request at least one image more than minimum to avoid having to wait for
    // the driver to complete internal operations, which would make us unable to acquire
    // another image to render to.
    uint32_t image_count = capabilities_.minImageCount + 1;

    // Make sure we do not exceed the maximum number of images. If the max is set to 0,
    // that is a special value saying there is no max.
    if (capabilities_.maxImageCount > 0 && image_count > capabilities_.maxImageCount) {
        image_count = capabilities_.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR create_info;
    create_info.surface = surface_;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format_->format;
    create_info.imageColorSpace = surface_format_->colorSpace;
    create_info.imageExtent = extent_;
    create_info.presentMode = present_mode_;

    // The amount of layers each image consists of. Should always be 1 except for
    // something like stereoscopic 3D.
    create_info.imageArrayLayers = 1;

    // Specifies what kind of operations we'll use the images in the swap chain for.
    // We're going to render directly to them, so they're used as a color attachment.
    // If we wanted to instead render to a separate image to perform post-processing,
    // then instead specify eTransferDst.
    create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    QueueFamilyIndices indices(physical_device_);
    uint32_t queue_family_indices[] = {indices.graphics_family(),
                                       indices.presentation_family()};

    // We need to specify how to handle swap chain images that will be used across
    // multiple queue families.
    if (indices.graphics_family() != indices.presentation_family()) {
        // If the queue families differ, then we'll be using concurrent sharing. Images
        // can be used across multiple queue families without explicit ownership
        // transfers.
        create_info.imageSharingMode = vk::SharingMode::eConcurrent;

        // Concurrent mode requires we specify in advance which queue families ownership
        // will be shared.
        create_info.queueFamilyIndexCount =
                static_cast<uint32_t>(std::size(queue_family_indices));
        create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        // If the graphics and presentation queue families are the same (which is the
        // case on most hardware), then we should stick to exclusive mode. An image is
        // owned by one queue family at a time and ownership must be explicitly
        // transferred before using it in another family. This option offers the best
        // performance.
        create_info.imageSharingMode = vk::SharingMode::eExclusive;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    // We can specify that a certain transform should be applied to our images in the
    // swap chain (like 90 degree rotation). However, we don't want any transforms
    // right now so we'll set it to the current transform.
    create_info.preTransform = capabilities_.currentTransform;

    // Specifies if the alpha channel should be used for blending with other windows in
    // in the window system. We'll almost never want to do this, hence opaque.
    create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    // We don't care about processing obscured pixels.
    create_info.clipped = true;

    // It's possible that our swap chain can become invalid or unoptimized while the
    // program is running (e.g. if the window is resized). In such a case, the swap
    // chain needs to be entirely recreated from scratch and a reference to the old one
    // must be specified in this field.
    create_info.oldSwapchain = old_swapchain;

    return create_info;
}
}  // namespace maseya::renderer