#ifndef COMPILE_SHADER_H
#define COMPILE_SHADER_H

#include <shaderc/shaderc.h>
#include <vulkan/vulkan.h>

VkResult compile_shader(VkDevice device, shaderc_compiler_t compiler,
                        shaderc_compile_options_t options, const char* source_text,
                        size_t source_text_size, shaderc_shader_kind shader_kind,
                        VkShaderModule* shader_module);

#endif