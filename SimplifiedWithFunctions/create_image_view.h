#ifndef CREATE_IMAGE_VIEW_H
#define CREATE_IMAGE_VIEW_H

// External APIs
#include <vulkan/vulkan.h>

VkResult create_image_view(VkDevice device, VkImage image, VkFormat format,
                           VkImageView* image_view);

#endif
