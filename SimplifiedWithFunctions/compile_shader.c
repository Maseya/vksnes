#include "compile_shader.h"

// Local headers
#include "report.h"

// Standard library
#include <string.h>

VkResult compile_shader(VkDevice device, shaderc_compiler_t compiler,
                        shaderc_compile_options_t options, const char* source_text,
                        size_t source_text_size, shaderc_shader_kind shader_kind,
                        VkShaderModule* shader_module) {
    VkResult result = VK_SUCCESS;

    shaderc_compilation_result_t compilation_result = shaderc_compile_into_spv(
            compiler, source_text, source_text_size, shader_kind, "", "main", options);

    shaderc_compilation_status status =
            shaderc_result_get_compilation_status(compilation_result);
    if (status != shaderc_compilation_status_success) {
        result = VK_ERROR_FEATURE_NOT_PRESENT;
        goto cleanup;
    }

    VkShaderModuleCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = shaderc_result_get_length(compilation_result),
            .pCode = (const uint32_t*)shaderc_result_get_bytes(compilation_result),
    };

    result = vkCreateShaderModule(device, &create_info, &callbacks, shader_module);

cleanup:
    shaderc_result_release(compilation_result);

    return result;
}