// External APIs
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Standard library
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    VkResult result = VK_SUCCESS;

    VkInstance instance = NULL;
    VkDevice device = NULL;

    VkCommandPool command_pool = VK_NULL_HANDLE;

    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory image_device_memory = VK_NULL_HANDLE;

    VkImageView image_view = VK_NULL_HANDLE;

    VkRenderPass render_pass = VK_NULL_HANDLE;

    VkFramebuffer framebuffer = VK_NULL_HANDLE;

    VkBuffer image_buffer = VK_NULL_HANDLE;
    VkDeviceMemory image_buffer_device_memory = VK_NULL_HANDLE;

    VkCommandBuffer command_buffer = NULL;

    void* pixel_data = NULL;

    // Create a Vulkan instance.
    const char* extensions[] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
    const char* layers[] = {"VK_LAYER_KHRONOS_validation"};
    VkInstanceCreateInfo instance_create_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .enabledLayerCount = 1,
            .ppEnabledLayerNames = layers,
            .enabledExtensionCount = 1,
            .ppEnabledExtensionNames = extensions,
    };
    result = vkCreateInstance(&instance_create_info, NULL, &instance);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Could not create a Vulkan instance.");
        goto cleanup;
    }

    // Retrieve the first physical device.
    uint32_t physical_device_count = 1;
    VkPhysicalDevice physical_device;
    result = vkEnumeratePhysicalDevices(instance, &physical_device_count,
                                        &physical_device);
    if (result < 0) {
        fprintf(stderr, "Could not retrieve any physical devices.");
        goto cleanup;
    } else if (result == VK_INCOMPLETE) {
        printf("Multiple GPUs detected. Picking first one found.");
    }

// Get graphics queue index.
#define MAX_QUEUE_FAMILY_COUNT (1 << 5)
    uint32_t queue_family_count = MAX_QUEUE_FAMILY_COUNT;
    VkQueueFamilyProperties queue_family_properties[MAX_QUEUE_FAMILY_COUNT];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                             queue_family_properties);

    uint32_t queue_family_index;
    bool queue_family_index_found = false;
    for (uint32_t i = 0; i < queue_family_count; i++) {
        if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queue_family_index = i;
            queue_family_index_found = true;
            break;
        }
    }

    if (!queue_family_index_found) {
        printf("Could not find queue family that supports graphics operations.");
        goto cleanup;
    }

    // Create device
    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo device_queue_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_family_index,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority,
    };

    VkDeviceCreateInfo device_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &device_queue_create_info,
    };

    result = vkCreateDevice(physical_device, &device_create_info, NULL, &device);
    if (result < 0) {
        goto cleanup;
    }

    VkQueue queue;
    vkGetDeviceQueue(device, queue_family_index, 0, &queue);

    // Create command pool
    VkCommandPoolCreateInfo command_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queue_family_index,
    };

    result =
            vkCreateCommandPool(device, &command_pool_create_info, NULL, &command_pool);

// Create image
#define IMAGE_WIDTH 0x200
#define IMAGE_HEIGHT IMAGE_WIDTH
#define IMAGE_FORMAT VK_FORMAT_B8G8R8A8_UNORM
    VkImageCreateInfo image_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .extent.width = IMAGE_WIDTH,
            .extent.height = IMAGE_HEIGHT,
            .extent.depth = 1,
            .mipLevels = 1,
            .arrayLayers = 1,
            .format = IMAGE_FORMAT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .samples = VK_SAMPLE_COUNT_1_BIT,
    };

    result = vkCreateImage(device, &image_create_info, NULL, &image);
    if (result < 0) {
        goto cleanup;
    }

    VkMemoryRequirements image_memory_requirements;
    vkGetImageMemoryRequirements(device, image, &image_memory_requirements);

    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device,
                                        &physical_device_memory_properties);

    uint32_t image_memory_type_index;
    bool image_memory_type_index_found = false;
    for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++) {
        if ((image_memory_requirements.memoryTypeBits & (1 << i)) &&
            (physical_device_memory_properties.memoryTypes[i].propertyFlags &
             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0) {
            image_memory_type_index = i;
            image_memory_type_index_found = true;
            break;
        }
    }

    if (!image_memory_type_index_found) {
        goto cleanup;
    }

    VkMemoryAllocateInfo image_memory_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = image_memory_requirements.size,
            .memoryTypeIndex = 0,
    };

    result = vkAllocateMemory(device, &image_memory_allocate_info, NULL,
                              &image_device_memory);
    if (result < 0) {
        goto cleanup;
    }

    result = vkBindImageMemory(device, image, image_device_memory, 0);
    if (result < 0) {
        goto cleanup;
    }

    // Create image view
    VkImageViewCreateInfo image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_B8G8R8A8_UNORM,
            .image = image,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1,
    };

    result = vkCreateImageView(device, &image_view_create_info, NULL, &image_view);
    if (result < 0) {
        goto cleanup;
    }

    // Create render pass
    VkAttachmentDescription color_attachment_description = {
            .format = VK_FORMAT_B8G8R8A8_UNORM,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    };

    VkAttachmentReference color_attachment_reference = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass_description = {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment_reference,
    };

    VkRenderPassCreateInfo render_pass_create_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &color_attachment_description,
            .subpassCount = 1,
            .pSubpasses = &subpass_description,
    };

    result = vkCreateRenderPass(device, &render_pass_create_info, NULL, &render_pass);
    if (result < 0) {
        goto cleanup;
    }

    // Create framebuffer
    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = render_pass,
            .attachmentCount = 1,
            .pAttachments = &image_view,
            .width = IMAGE_WIDTH,
            .height = IMAGE_HEIGHT,
            .layers = 1,
    };

    result = vkCreateFramebuffer(device, &framebuffer_create_info, NULL, &framebuffer);
    if (result < 0) {
        goto cleanup;
    }

    // Create image buffer.
    VkBufferCreateInfo buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = image_memory_allocate_info.allocationSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    result = vkCreateBuffer(device, &buffer_create_info, NULL, &image_buffer);
    if (result < 0) {
        goto cleanup;
    }

    VkMemoryRequirements image_buffer_memory_requirements;
    vkGetBufferMemoryRequirements(device, image_buffer,
                                  &image_buffer_memory_requirements);

    uint32_t image_buffer_memory_type_index;
    bool image_buffer_memory_type_index_found = false;
    for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++) {
        if ((image_buffer_memory_requirements.memoryTypeBits & (1 << i)) &&
            (physical_device_memory_properties.memoryTypes[i].propertyFlags &
             (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) != 0) {
            image_buffer_memory_type_index = i;
            image_buffer_memory_type_index_found = true;
            break;
        }
    }

    if (!image_buffer_memory_type_index_found) {
        goto cleanup;
    }

    VkMemoryAllocateInfo image_buffer_memory_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = image_buffer_memory_requirements.size,
            .memoryTypeIndex = image_buffer_memory_type_index,
    };

    result = vkAllocateMemory(device, &image_buffer_memory_allocate_info, NULL,
                              &image_buffer_device_memory);
    if (result < 0) {
        goto cleanup;
    }

    result = vkBindBufferMemory(device, image_buffer, image_buffer_device_memory, 0);
    if (result < 0) {
        goto cleanup;
    }

    // Allocate command buffer.
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = command_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };

    result = vkAllocateCommandBuffers(device, &command_buffer_allocate_info,
                                      &command_buffer);
    if (result < 0) {
        goto cleanup;
    }

    // Create command to clear image to single color.
    VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    VkClearValue clear_value = {
            .color = {0.2f, 0.4f, 0.7f, 1.0f},  // BGRA color
    };

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = render_pass,
            .framebuffer = framebuffer,
            .renderArea.offset = {0, 0},
            .renderArea.extent = {IMAGE_WIDTH, IMAGE_HEIGHT},
            .clearValueCount = 1,
            .pClearValues = &clear_value,
    };

    VkBufferImageCopy region = {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,

            .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .imageSubresource.mipLevel = 0,
            .imageSubresource.baseArrayLayer = 0,
            .imageSubresource.layerCount = 1,

            .imageOffset = {0, 0, 0},
            .imageExtent = {IMAGE_WIDTH, IMAGE_HEIGHT, 1},
    };

    result = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
    if (result < 0) {
        goto cleanup;
    }

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(command_buffer);

    vkCmdCopyImageToBuffer(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image_buffer, 1, &region);

    result = vkEndCommandBuffer(command_buffer);
    if (result < 0) {
        goto cleanup;
    }

    // Submit the command.
    VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &command_buffer,
    };

    result = vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    if (result < 0) {
        goto cleanup;
    }

    // Wait for command to finish.
    result = vkQueueWaitIdle(queue);
    if (result < 0) {
        goto cleanup;
    }

    // Copy pixel data to source array
    void* image_buffer_data;
    result = vkMapMemory(device, image_buffer_device_memory, 0,
                         image_buffer_memory_requirements.size, 0, &image_buffer_data);
    if (result < 0) {
        goto cleanup;
    }

    pixel_data = malloc((size_t)image_buffer_memory_requirements.size);
    if (pixel_data == NULL) {
        goto cleanup;
    }

    memcpy(pixel_data, image_buffer_data,
           (size_t)image_buffer_memory_requirements.size);

    vkUnmapMemory(device, image_buffer_device_memory);

    // Write pixel data to I/O file.
    int res = stbi_write_png("image.png", IMAGE_WIDTH, IMAGE_HEIGHT, 4, pixel_data,
                             IMAGE_WIDTH * 4);

cleanup:
    if (result < 0) {
        fprintf(stderr,
                "A Vulkan API call failed somewhere. This usually indicates an "
                "incompatible system.");
    }

    free(pixel_data);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);

    vkFreeMemory(device, image_buffer_device_memory, NULL);
    vkDestroyBuffer(device, image_buffer, NULL);

    vkDestroyFramebuffer(device, framebuffer, NULL);

    vkDestroyRenderPass(device, render_pass, NULL);

    vkDestroyImageView(device, image_view, NULL);

    vkFreeMemory(device, image_device_memory, NULL);
    vkDestroyImage(device, image, NULL);

    vkDestroyCommandPool(device, command_pool, NULL);

    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);

    return result;
}