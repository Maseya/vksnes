#ifndef COPY_IMAGE_TO_BUFFER_H
#define COPY_IMAGE_TO_BUFFER_H

// External APIs
#include <vulkan/vulkan.h>

void copy_image_to_buffer(VkCommandBuffer command_buffer, VkImage image, uint32_t width,
                          uint32_t height, VkBuffer image_buffer);

#endif