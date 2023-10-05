#include "PipelineManager.hxx"

#include "Compiler.hxx"
#include "UniqueObject.hxx"
#include "shader_helper.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vksnes {
const static std::unordered_map<PixelFormat, const char*> formats = {
        {PixelFormat::Format1Bpp8x8, "FORMAT_1BPP_8X8"},
        {PixelFormat::Format2BppNes, "FORMAT_2BPP_NES"},
        {PixelFormat::Format2BppGb, "FORMAT_2BPP_GB"},
        {PixelFormat::Format2BppNgp, "FORMAT_2BPP_NGP"},
        {PixelFormat::Format2BppVb, "FORMAT_2BPP_VB"},
        {PixelFormat::Format3BppSnes, "FORMAT_3BPP_SNES"},
        {PixelFormat::Format3Bpp8x8, "FORMAT_3BPP_8X8"},
        {PixelFormat::Format4BppSnes, "FORMAT_4BPP_SNES"},
        {PixelFormat::Format4BppGba, "FORMAT_4BPP_GBA"},
        {PixelFormat::Format4BppSms, "FORMAT_4BPP_SMS"},
        {PixelFormat::Format4BppMsx2, "FORMAT_4BPP_MSX2"},
        {PixelFormat::Format4Bpp8x8, "FORMAT_4BPP_8X8"},
        {PixelFormat::Format8BppSnes, "FORMAT_8BPP_SNES"},
        {PixelFormat::Format8BppMode7, "FORMAT_8BPP_MODE7"},
};

class ShaderModuleDestroyer {
public:
    ShaderModuleDestroyer(VkDevice device) : device_(device) {}

    void operator()(VkShaderModule shader_module) const noexcept {
        vkDestroyShaderModule(device_, shader_module, nullptr);
    }

private:
    VkDevice device_;
};

PipelineManager::PipelineManager(std::nullptr_t) noexcept
        : overlay_pipeline_(nullptr),
          checker_pipeline_(nullptr),
          gradient_pipeline_(nullptr),
          palette_pipeline_(nullptr),
          gfx_pipelines_(),
          obj_pipelines_(),
          line_pipeline_(nullptr) {}

PipelineManager::PipelineManager(VkDevice device, VkRenderPass flat_render_pass)
        : PipelineManager(nullptr) {
    class CreateShader {
    public:
        CreateShader(VkDevice device) : device_(device), compiler_() {}

        VkShaderModule operator()(const char* path,
                                  const std::vector<Define>& defines = {}) {
            ShaderKind shader_kind = get_shader_kind_from_file_name(path);
            auto spirv_code = compiler_.compile_shader(path, shader_kind, defines);
            return create_shader_module(device_, spirv_code);
        }

    private:
        VkDevice device_;
        Compiler compiler_;
    } create_shader(device);

    using UniqueShaderModule = UniqueObject<VkShaderModule, ShaderModuleDestroyer>;
    UniqueShaderModule vert_shader(VK_NULL_HANDLE, device);
    UniqueShaderModule frag_shader(VK_NULL_HANDLE, device);

    static constexpr descriptor_set_layout_binding overlay_bindings[] = {
            {
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
    };

    std::vector<Define> defines({{"TEX_COORDS", ""}});
    vert_shader.reset(create_shader("shaders/flat.vert", defines));
    frag_shader.reset(create_shader("shaders/overlay.frag"));
    overlay_pipeline_ = Pipeline(device, flat_render_pass, overlay_bindings,
                                 vert_shader.get(), frag_shader.get());

    static constexpr descriptor_set_layout_binding checker_bindings[] = {
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
    };

    vert_shader.reset(create_shader("shaders/flat.vert"));
    frag_shader.reset(create_shader("shaders/checker.frag"));
    checker_pipeline_ = Pipeline(device, flat_render_pass, checker_bindings,
                                 vert_shader.get(), frag_shader.get());

    static constexpr descriptor_set_layout_binding gradient_bindings[] = {
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
    };

    frag_shader.reset(create_shader("shaders/gradient.frag"));
    gradient_pipeline_ = Pipeline(device, flat_render_pass, gradient_bindings,
                                  vert_shader.get(), frag_shader.get());

    static constexpr descriptor_set_layout_binding palette_bindings[] = {
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
    };

    frag_shader.reset(create_shader("shaders/palette.frag"));
    palette_pipeline_ = Pipeline(device, flat_render_pass, palette_bindings,
                                 vert_shader.get(), frag_shader.get());

    static constexpr descriptor_set_layout_binding gfx_bindings[] = {
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
    };

    defines[0].name = "GFX_FORMAT";
    for (const auto& pair : formats) {
        defines[0].value = pair.second;
        frag_shader.reset(create_shader("shaders/gfx.frag", defines));
        gfx_pipelines_.emplace(pair.first,
                               Pipeline(device, flat_render_pass, gfx_bindings,
                                        vert_shader.get(), frag_shader.get()));
    }

    static constexpr descriptor_set_layout_binding obj_bindings[] = {
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
    };

    for (const auto& pair : formats) {
        defines[0].value = pair.second;
        frag_shader.reset(create_shader("shaders/obj.frag", defines));
        obj_pipelines_[false].emplace(pair.first,
                                      Pipeline(device, flat_render_pass, obj_bindings,
                                               vert_shader.get(), frag_shader.get()));
    }

    defines.push_back({{"MAP16_TILEMAP"}});
    for (const auto& pair : formats) {
        defines[0].value = pair.second;
        frag_shader.reset(create_shader("shaders/obj.frag", defines));
        obj_pipelines_[true].emplace(pair.first,
                                     Pipeline(device, flat_render_pass, obj_bindings,
                                              vert_shader.get(), frag_shader.get()));
    }

    static constexpr descriptor_set_layout_binding map_bindings[] = {
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
    };

    for (const auto& pair : formats) {
        defines[0].value = pair.second;
        frag_shader.reset(create_shader("shaders/map.frag", defines));
        map_pipelines_[true][false].emplace(
                pair.first, Pipeline(device, flat_render_pass, map_bindings,
                                     vert_shader.get(), frag_shader.get()));
    }

    defines.back().name = "WORD_INDEX";
    for (const auto& pair : formats) {
        defines[0].value = pair.second;
        frag_shader.reset(create_shader("shaders/map.frag", defines));
        map_pipelines_[false][true].emplace(
                pair.first, Pipeline(device, flat_render_pass, map_bindings,
                                     vert_shader.get(), frag_shader.get()));
    }

    defines.push_back({"MAP16_TILEMAP", ""});
    for (const auto& pair : formats) {
        defines[0].value = pair.second;
        frag_shader.reset(create_shader("shaders/map.frag", defines));
        map_pipelines_[true][true].emplace(
                pair.first, Pipeline(device, flat_render_pass, map_bindings,
                                     vert_shader.get(), frag_shader.get()));
    }

    defines.erase(defines.end() - 2, defines.end());
    for (const auto& pair : formats) {
        defines[0].value = pair.second;
        frag_shader.reset(create_shader("shaders/map.frag", defines));
        map_pipelines_[false][false].emplace(
                pair.first, Pipeline(device, flat_render_pass, map_bindings,
                                     vert_shader.get(), frag_shader.get()));
    }

    static constexpr descriptor_set_layout_binding chr_bindings[] = {
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
    };

    std::vector<VkVertexInputBindingDescription> binding_descriptions(2);
    binding_descriptions[0].binding = 0;
    binding_descriptions[0].stride = static_cast<uint32_t>(sizeof(VkExtent2D));
    binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
    binding_descriptions[1].binding = 1;
    binding_descriptions[1].stride = static_cast<uint32_t>(sizeof(uint32_t));
    binding_descriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    std::vector<VkVertexInputAttributeDescription> attribute_descriptions(2);
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32_UINT;
    attribute_descriptions[0].offset = 0;
    attribute_descriptions[1].binding = 1;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32_UINT;
    attribute_descriptions[1].offset = 0;

    vert_shader.reset(create_shader("shaders/chr.vert"));
    for (const auto& pair : formats) {
        defines[0].value = pair.second;
        frag_shader.reset(create_shader("shaders/chr.frag", defines));
        chr_pipelines_.emplace(pair.first,
                               Pipeline(device, flat_render_pass, chr_bindings,
                                        vert_shader.get(), frag_shader.get(),
                                        binding_descriptions, attribute_descriptions));
    }

    static constexpr descriptor_set_layout_binding line_bindings[] = {
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT,
            },
            {
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
            },
    };

    defines.front() = {"TEX_COORDS", ""};
    vert_shader.reset(create_shader("shaders/line.vert", defines));
    frag_shader.reset(create_shader("shaders/line.frag", defines));

    binding_descriptions[0].binding = 0;
    binding_descriptions[0].stride = 4 * static_cast<uint32_t>(sizeof(float));
    binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
    binding_descriptions[1].binding = 1;
    binding_descriptions[1].stride = 2 * static_cast<uint32_t>(sizeof(float));
    binding_descriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    attribute_descriptions.resize(4);
    for (size_t i = 0; i < 2; i++) {
        attribute_descriptions[i].binding = 0;
        attribute_descriptions[i].location = static_cast<uint32_t>(i);
        attribute_descriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[i].offset = static_cast<uint32_t>(2 * i * sizeof(float));

        attribute_descriptions[i + 2].binding = 1;
        attribute_descriptions[i + 2].location = static_cast<uint32_t>(i + 2);
        attribute_descriptions[i + 2].format = VK_FORMAT_R32_SFLOAT;
        attribute_descriptions[i + 2].offset = static_cast<uint32_t>(i * sizeof(float));
    }

    line_pipeline_ =
            Pipeline(device, flat_render_pass, line_bindings, vert_shader.get(),
                     frag_shader.get(), binding_descriptions, attribute_descriptions);
}
}  // namespace maseya::vksnes