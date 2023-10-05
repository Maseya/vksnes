#include "copy_image_to_buffer.h"

void copy_image_to_buffer(VkCommandBuffer command_buffer, VkImage image, uint32_t width,
                          uint32_t height, VkBuffer image_buffer) {
    VkBufferImageCopy region = {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,

            .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .imageSubresource.mipLevel = 0,
            .imageSubresource.baseArrayLayer = 0,
            .imageSubresource.layerCount = 1,

            .imageOffset = {0, 0, 0},
            .imageExtent = {width, height, 1},
    };

    vkCmdCopyImageToBuffer(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image_buffer, 1, &region);
}
