#include "TestPipeline.hxx"

// Local headers
#include "VulkanError.hxx"
#include "shader_module_from_file.hxx"

namespace maseya::renderer {
namespace {
vk::UniqueRenderPass create_render_pass(vk::Device device, vk::Format format);

vk::UniquePipelineLayout create_pipeline_layout(vk::Device device);

vk::UniquePipeline create_graphics_pipeline(vk::Device device,
                                            vk::PipelineLayout pipeline_layout,
                                            vk::RenderPass render_pass);
}  // namespace

TestPipeline::TestPipeline(vk::Device device, vk::Format format)
        : render_pass_(create_render_pass(device, format)),
          pipeline_layout_(create_pipeline_layout(device)),
          graphics_pipeline_(
                  create_graphics_pipeline(device, *pipeline_layout_, *render_pass_)) {}

void TestPipeline::render(RenderTarget& render_target) const {
    const static vk::ClearColorValue clear_color =
            vk::ClearColorValue(std::array<float, 4>({0.3f, 0.5f, 0.7f, 1.0f}));

    if (!render_target) {
        return;
    }

    render_target.begin(render_pass(), clear_color);

    vk::CommandBuffer command_buffer = render_target.command_buffer();
    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphics_pipeline_);
    command_buffer.draw(3, 1, 0, 0);

    render_target.end();
}

namespace {
vk::UniqueRenderPass create_render_pass(vk::Device device, vk::Format format) {
    vk::AttachmentDescription color_attachment;
    color_attachment.format = format;
    color_attachment.samples = vk::SampleCountFlagBits::e1;
    color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
    color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
    color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    color_attachment.initialLayout = vk::ImageLayout::eUndefined;
    color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    vk::SubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = static_cast<vk::AccessFlagBits>(0);
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo create_info;
    create_info.attachmentCount = 1;
    create_info.pAttachments = &color_attachment;
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass;
    create_info.dependencyCount = 1;
    create_info.pDependencies = &dependency;

    return device.createRenderPassUnique(create_info);
}

vk::UniquePipelineLayout create_pipeline_layout(vk::Device device) {
    vk::PipelineLayoutCreateInfo pipeline_layout_info;
    return device.createPipelineLayoutUnique(pipeline_layout_info);
}

vk::UniquePipeline create_graphics_pipeline(vk::Device device,
                                            vk::PipelineLayout pipeline_layout,
                                            vk::RenderPass render_pass) {
    vk::UniqueShaderModule vertex_shader(
            shader_module_from_file(device, "shaders/triangle.spv"));
    vk::UniqueShaderModule fragment_shader(
            shader_module_from_file(device, "shaders/blank.spv"));

    vk::PipelineShaderStageCreateInfo vert_shader_stage_info;
    vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
    vert_shader_stage_info.module = *vertex_shader;
    vert_shader_stage_info.pName = "main";

    vk::PipelineShaderStageCreateInfo frag_shader_stage_info;
    frag_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
    frag_shader_stage_info.module = *fragment_shader;
    frag_shader_stage_info.pName = "main";

    vk::PipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info,
                                                         frag_shader_stage_info};

    vk::PipelineVertexInputStateCreateInfo vertex_input_info;

    vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_info;
    input_assembly_state_info.topology = vk::PrimitiveTopology::eTriangleStrip;

    vk::PipelineViewportStateCreateInfo viewport_state_info;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterizer_state_info;
    rasterizer_state_info.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisampling_info;
    multisampling_info.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling_info.minSampleShading = 1.0f;

    vk::PipelineColorBlendAttachmentState color_blend_attachment;
    color_blend_attachment.colorWriteMask =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eZero;
    color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;
    color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo color_blend_info;
    color_blend_info.logicOp = vk::LogicOp::eCopy;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = &color_blend_attachment;

    constexpr static vk::DynamicState dynamic_states[] = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor,
    };
    vk::PipelineDynamicStateCreateInfo dynamic_state_info;
    dynamic_state_info.dynamicStateCount =
            static_cast<uint32_t>(std::size(dynamic_states));
    dynamic_state_info.pDynamicStates = dynamic_states;

    vk::GraphicsPipelineCreateInfo graphics_pipeline_info;
    graphics_pipeline_info.stageCount = static_cast<uint32_t>(std::size(shader_stages));
    graphics_pipeline_info.pStages = shader_stages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly_state_info;
    graphics_pipeline_info.pViewportState = &viewport_state_info;
    graphics_pipeline_info.pRasterizationState = &rasterizer_state_info;
    graphics_pipeline_info.pMultisampleState = &multisampling_info;
    graphics_pipeline_info.pColorBlendState = &color_blend_info;
    graphics_pipeline_info.pDynamicState = &dynamic_state_info;
    graphics_pipeline_info.layout = pipeline_layout;
    graphics_pipeline_info.renderPass = render_pass;
    graphics_pipeline_info.basePipelineIndex = -1;

    return device.createGraphicsPipelineUnique({}, graphics_pipeline_info).value;
}
}  // namespace
}  // namespace maseya::renderer