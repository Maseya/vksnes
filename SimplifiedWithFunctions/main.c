// Local headers
#include "allocate_command_buffer.h"
#include "begin_command_buffer.h"
#include "begin_render_pass.h"
#include "compile_shader.h"
#include "copy_image_to_buffer.h"
#include "create_buffer.h"
#include "create_command_pool.h"
#include "create_device.h"
#include "create_framebuffer.h"
#include "create_image.h"
#include "create_image_view.h"
#include "create_instance.h"
#include "create_pipeline.h"
#include "create_pipeline_layout.h"
#include "create_render_pass.h"
#include "get_graphics_queue_family_index.h"
#include "get_memory_data.h"
#include "pick_physical_device.h"
#include "read_text_file.h"
#include "report.h"
#include "set_viewport_and_scissor.h"
#include "submit_command.h"

// STB Image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// External APIs
#include <shaderc/shaderc.h>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

// Standard library
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMAGE_WIDTH 0x200
#define IMAGE_HEIGHT IMAGE_WIDTH
#define IMAGE_FORMAT VK_FORMAT_B8G8R8A8_UNORM

#define SHADER_DIR "../SnesRenderLib/shaders/"

static const char* flat_vert_shader_path = SHADER_DIR "flat.vert";
static const char* checker_frag_shader_path = SHADER_DIR "checker.frag";

typedef struct {
    float colors[2][4];
    float size[2];
} checkerboard_pattern;

static VkResult create_checkerboard_descriptor_set_layout(
        VkDevice device, VkDescriptorSetLayout* layout);
static VkResult create_checkerboard_descriptor_pool(VkDevice device,
                                                    VkDescriptorPool* descriptor_pool);
static VkResult allocate_checkerboard_descriptor_set(VkDevice device,
                                                     VkDescriptorPool descriptor_pool,
                                                     VkDescriptorSetLayout layout,
                                                     VkDescriptorSet* descriptor_set);

int main(int argc, char* argv[]) {
    VkResult result = VK_SUCCESS;

    VkInstance instance = NULL;
    VkDevice device = NULL;

    shaderc_compiler_t compiler = NULL;
    shaderc_compile_options_t options = NULL;

    char* flat_vert_shader_source = NULL;
    char* checker_frag_shader_source = NULL;

    VkShaderModule flat_vert_shader_module = VK_NULL_HANDLE;
    VkShaderModule checker_frag_shader_module = VK_NULL_HANDLE;

    VkRenderPass render_pass = VK_NULL_HANDLE;
    VkDescriptorSetLayout checkerboard_descriptor_set_layout = VK_NULL_HANDLE;
    VkPipelineLayout checkerboard_pipeline_layout = VK_NULL_HANDLE;

    VkPipeline checkerboard_pipeline = VK_NULL_HANDLE;

    VkBuffer checkerboard_pattern_buffer = VK_NULL_HANDLE;
    VkDeviceMemory checkerboard_pattern_buffer_memory = VK_NULL_HANDLE;

    VkDescriptorPool checkerboard_descriptor_pool = VK_NULL_HANDLE;
    VkDescriptorSet checkerboard_descriptor_set = VK_NULL_HANDLE;

    VkCommandPool command_pool = VK_NULL_HANDLE;

    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory image_device_memory = VK_NULL_HANDLE;

    VkImageView image_view = VK_NULL_HANDLE;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;

    VkBuffer image_buffer = VK_NULL_HANDLE;
    VkDeviceMemory image_buffer_device_memory = VK_NULL_HANDLE;

    VkCommandBuffer command_buffer = NULL;

    void* pixel_data = NULL;

    result = create_instance(&instance);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateInstance");

    VkPhysicalDevice physical_device;
    result = pick_physical_device(instance, &physical_device);
    if (result < 0) {
        goto cleanup;
    }

    report("vkEnumeratePhysicalDevices");

    uint32_t queue_family_index;
    result = get_graphics_queue_family_index(physical_device, &queue_family_index);
    if (result < 0) {
        goto cleanup;
    }

    report("vkGetPhysicalDeviceQueueFamilyProperties");

    result = create_device(physical_device, queue_family_index, &device);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateDevice");

    compiler = shaderc_compiler_initialize();
    if (compiler == NULL) {
        result = VK_ERROR_FEATURE_NOT_PRESENT;
        goto cleanup;
    }

    options = shaderc_compile_options_initialize();
    if (options == NULL) {
        result = VK_ERROR_FEATURE_NOT_PRESENT;
        goto cleanup;
    }

    shaderc_compile_options_set_source_language(options, shaderc_source_language_glsl);
    shaderc_compile_options_set_optimization_level(
            options, shaderc_optimization_level_performance);
    shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan,
                                           shaderc_env_version_vulkan_1_0);

    size_t flat_vert_shader_source_size;
    result = read_text_file(flat_vert_shader_path, &flat_vert_shader_source,
                            &flat_vert_shader_source_size);
    if (result < 0) {
        goto cleanup;
    }

    size_t checker_frag_shader_source_size;
    result = read_text_file(checker_frag_shader_path, &checker_frag_shader_source,
                            &checker_frag_shader_source_size);
    if (result < 0) {
        goto cleanup;
    }

    result = compile_shader(device, compiler, options, flat_vert_shader_source,
                            flat_vert_shader_source_size, shaderc_glsl_vertex_shader,
                            &flat_vert_shader_module);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateShaderModule");

    result = compile_shader(device, compiler, options, checker_frag_shader_source,
                            checker_frag_shader_source_size,
                            shaderc_glsl_fragment_shader, &checker_frag_shader_module);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateShaderModule");

    result = create_render_pass(device, IMAGE_FORMAT, VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, &render_pass);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateRenderPass");

    result = create_checkerboard_descriptor_set_layout(
            device, &checkerboard_descriptor_set_layout);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateDescriptorSetLayout");

    result = create_pipeline_layout(device, &checkerboard_descriptor_set_layout, 1,
                                    &checkerboard_pipeline_layout);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreatePipelineLayout");

    result = create_pipeline(device, render_pass, checkerboard_pipeline_layout,
                             flat_vert_shader_module, checker_frag_shader_module,
                             &checkerboard_pipeline);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreatePipelines");

    VkDeviceSize checkerboard_pattern_buffer_allocation_size;
    result = create_buffer(
            physical_device, device, sizeof(checkerboard_pattern),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &checkerboard_pattern_buffer, &checkerboard_pattern_buffer_memory,
            &checkerboard_pattern_buffer_allocation_size);
    if (result < 0) {
        goto cleanup;
    }

    report("vkAllocateMemory");

    checkerboard_pattern* checker_pattern;
    result = vkMapMemory(device, checkerboard_pattern_buffer_memory, 0,
                         sizeof(checkerboard_pattern), 0, &checker_pattern);
    if (result < 0) {
        goto cleanup;
    }

    checker_pattern->colors[0][0] = 0.2f;
    checker_pattern->colors[0][1] = 0.7f;
    checker_pattern->colors[0][2] = 0.8f;
    checker_pattern->colors[0][3] = 1.0f;

    checker_pattern->colors[1][0] = 0.5f;
    checker_pattern->colors[1][1] = 0.2f;
    checker_pattern->colors[1][2] = 0.3f;
    checker_pattern->colors[1][3] = 1.0f;

    checker_pattern->size[0] = 0x20;
    checker_pattern->size[1] = 0x10;

    vkUnmapMemory(device, checkerboard_pattern_buffer_memory);

    result = create_checkerboard_descriptor_pool(device, &checkerboard_descriptor_pool);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateDescriptorPool");

    result = allocate_checkerboard_descriptor_set(device, checkerboard_descriptor_pool,
                                                  checkerboard_descriptor_set_layout,
                                                  &checkerboard_descriptor_set);
    if (result < 0) {
        goto cleanup;
    }

    report("vkAllocateDescriptorSets");

    VkDescriptorBufferInfo buffer_info = {
            .buffer = checkerboard_pattern_buffer,
            .offset = 0,
            .range = sizeof(checkerboard_pattern),
    };

    VkWriteDescriptorSet descriptor_write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = checkerboard_descriptor_set,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .pBufferInfo = &buffer_info,
    };

    vkUpdateDescriptorSets(device, 1, &descriptor_write, 0, NULL);

    report("vkUpdateDescriptorSets");

    VkQueue queue;
    vkGetDeviceQueue(device, queue_family_index, 0, &queue);
    report("vkGetDeviceQueue");

    result = create_command_pool(device, queue_family_index, &command_pool);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateCommandPool");

    VkDeviceSize image_allocation_size;
    result = create_image(physical_device, device, IMAGE_WIDTH, IMAGE_HEIGHT,
                          IMAGE_FORMAT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image,
                          &image_device_memory, &image_allocation_size);
    if (result < 0) {
        goto cleanup;
    }

    report("vkAllocateMemory");

    result = create_image_view(device, image, IMAGE_FORMAT, &image_view);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateImageView");

    result = create_framebuffer(device, IMAGE_WIDTH, IMAGE_HEIGHT, image_view,
                                render_pass, &framebuffer);
    if (result < 0) {
        goto cleanup;
    }

    report("vkCreateFramebuffer");

    VkDeviceSize image_buffer_allocation_size;
    result = create_buffer(
            physical_device, device, image_allocation_size,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &image_buffer, &image_buffer_device_memory, &image_buffer_allocation_size);
    if (result < 0) {
        goto cleanup;
    }

    report("vkAllocateMemory");

    result = allocate_command_buffer(device, command_pool, &command_buffer);
    if (result < 0) {
        goto cleanup;
    }

    report("vkAllocateCommandBuffer");

    result = begin_command_buffer(command_buffer);
    if (result < 0) {
        goto cleanup;
    }

    set_viewport_and_scissor(command_buffer, IMAGE_WIDTH, IMAGE_HEIGHT);

    float clear_color[4] = {0.3f, 0.5f, 0.8f, 1.0f};
    begin_render_pass(command_buffer, render_pass, framebuffer, IMAGE_WIDTH,
                      IMAGE_HEIGHT, clear_color);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      checkerboard_pipeline);

    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            checkerboard_pipeline_layout, 0, 1,
                            &checkerboard_descriptor_set, 0, NULL);

    vkCmdDraw(command_buffer, 4, 1, 0, 0);

    vkCmdEndRenderPass(command_buffer);
    copy_image_to_buffer(command_buffer, image, IMAGE_WIDTH, IMAGE_HEIGHT,
                         image_buffer);

    report("Command setting");

    result = vkEndCommandBuffer(command_buffer);
    if (result < 0) {
        goto cleanup;
    }

    report("vkEndCommandBuffer");

    result = submit_command(queue, command_buffer, VK_NULL_HANDLE);
    if (result < 0) {
        goto cleanup;
    }

    report("vkQueueSubmit");

    // Wait for command to finish.
    result = vkQueueWaitIdle(queue);
    if (result < 0) {
        goto cleanup;
    }

    report("vkQueueWaitIdle");

    pixel_data = malloc((size_t)image_allocation_size);
    if (pixel_data == NULL) {
        result = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto cleanup;
    }

    // Copy pixel data to source array
    result = get_memory_data(device, image_buffer_device_memory,
                             image_buffer_allocation_size, pixel_data);
    if (result < 0) {
        goto cleanup;
    }

    if (!stbi_write_png("image.png", IMAGE_WIDTH, IMAGE_HEIGHT, 4, pixel_data,
                        IMAGE_WIDTH * 4)) {
        result = VK_ERROR_FEATURE_NOT_PRESENT;
        goto cleanup;
    }

cleanup:
    if (result < 0) {
        fprintf(stderr,
                "A Vulkan API call failed somewhere. This usually indicates an "
                "incompatible system.");
    }

    free(pixel_data);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);

    vkFreeMemory(device, image_buffer_device_memory, &callbacks);
    vkDestroyBuffer(device, image_buffer, &callbacks);

    vkDestroyFramebuffer(device, framebuffer, &callbacks);
    vkDestroyRenderPass(device, render_pass, &callbacks);
    vkDestroyImageView(device, image_view, &callbacks);

    vkFreeMemory(device, image_device_memory, &callbacks);
    vkDestroyImage(device, image, &callbacks);

    vkFreeDescriptorSets(device, checkerboard_descriptor_pool, 1,
                         &checkerboard_descriptor_set);
    vkDestroyDescriptorPool(device, checkerboard_descriptor_pool, &callbacks);

    vkDestroyCommandPool(device, command_pool, &callbacks);

    vkFreeMemory(device, checkerboard_pattern_buffer_memory, &callbacks);
    vkDestroyBuffer(device, checkerboard_pattern_buffer, &callbacks);

    vkDestroyPipeline(device, checkerboard_pipeline, &callbacks);
    vkDestroyPipelineLayout(device, checkerboard_pipeline_layout, &callbacks);
    vkDestroyDescriptorSetLayout(device, checkerboard_descriptor_set_layout,
                                 &callbacks);

    vkDestroyShaderModule(device, checker_frag_shader_module, &callbacks);
    vkDestroyShaderModule(device, flat_vert_shader_module, &callbacks);

    free(checker_frag_shader_source);
    free(flat_vert_shader_source);

    shaderc_compile_options_release(options);
    shaderc_compiler_release(compiler);

    vkDestroyDevice(device, &callbacks);
    vkDestroyInstance(instance, &callbacks);

    return result;
}

VkResult create_checkerboard_descriptor_set_layout(VkDevice device,
                                                   VkDescriptorSetLayout* layout) {
    VkDescriptorSetLayoutBinding pattern_buffer_binding = {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    };

    VkDescriptorSetLayoutCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &pattern_buffer_binding,
    };

    return vkCreateDescriptorSetLayout(device, &create_info, &callbacks, layout);
}

VkResult create_checkerboard_descriptor_pool(VkDevice device,
                                             VkDescriptorPool* descriptor_pool) {
    VkDescriptorPoolSize pool_size = {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
    };

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            .maxSets = 1,
            .poolSizeCount = 1,
            .pPoolSizes = &pool_size,
    };

    return vkCreateDescriptorPool(device, &descriptor_pool_create_info, &callbacks,
                                  descriptor_pool);
}

VkResult allocate_checkerboard_descriptor_set(VkDevice device,
                                              VkDescriptorPool descriptor_pool,
                                              VkDescriptorSetLayout layout,
                                              VkDescriptorSet* descriptor_set) {
    VkDescriptorSetAllocateInfo allocate_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptor_pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &layout,
    };

    return vkAllocateDescriptorSets(device, &allocate_info, descriptor_set);
}