#include "PalettePipeline.hxx"

// Local headers
#include "RenderTarget.hxx"
#include "shader_module_from_file.hxx"

namespace maseya::renderer {
namespace {
vk::UniqueRenderPass create_render_pass(vk::Device device, vk::Format format);

vk::UniqueDescriptorSetLayout create_descriptor_set_layout(vk::Device device);

vk::UniquePipelineLayout create_pipeline_layout(
        vk::Device device, const vk::DescriptorSetLayout& descriptor_set_layout);

vk::UniquePipeline create_graphics_pipeline(vk::Device device,
                                            vk::PipelineLayout pipeline_layout,
                                            vk::RenderPass render_pass);
}  // namespace

PalettePipeline::PalettePipeline(vk::Device device, vk::Format format)
        : render_pass_(create_render_pass(device, format)),
          descriptor_set_layout_(create_descriptor_set_layout(device)),
          pipeline_layout_(create_pipeline_layout(device, *descriptor_set_layout_)),
          graphics_pipeline_(
                  create_graphics_pipeline(device, *pipeline_layout_, *render_pass_)) {}

void PalettePipeline::render(RenderTarget& render_target) const {
    const static vk::ClearColorValue clear_color =
            vk::ClearColorValue(std::array<float, 4>({0.3f, 0.5f, 0.7f, 1.0f}));

    if (!render_target) {
        return;
    }

    render_target.begin(*render_pass_, clear_color);

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

    // TODO(nrg): Is this the best layout? Palette can either be presented to screen or
    // be used as texture for post-processing.
    color_attachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

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

vk::UniqueDescriptorSetLayout create_descriptor_set_layout(vk::Device device) {
    vk::DescriptorSetLayoutBinding layout_bindings[3];
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = vk::ShaderStageFlagBits::eVertex;
    layout_bindings[0].pImmutableSamplers = nullptr;

    layout_bindings[1].binding = 1;
    layout_bindings[1].descriptorType = vk::DescriptorType::eStorageBuffer;
    layout_bindings[1].descriptorCount = 1;
    layout_bindings[1].stageFlags = vk::ShaderStageFlagBits::eFragment;
    layout_bindings[1].pImmutableSamplers = nullptr;

    layout_bindings[2].binding = 2;
    layout_bindings[2].descriptorType = vk::DescriptorType::eUniformBuffer;
    layout_bindings[2].descriptorCount = 1;
    layout_bindings[2].stageFlags = vk::ShaderStageFlagBits::eFragment;
    layout_bindings[2].pImmutableSamplers = nullptr;

    vk::DescriptorSetLayoutCreateInfo create_info;
    create_info.bindingCount = static_cast<uint32_t>(std::size(layout_bindings));
    create_info.pBindings = layout_bindings;

    return device.createDescriptorSetLayoutUnique(create_info);
}

vk::UniquePipelineLayout create_pipeline_layout(
        vk::Device device, const vk::DescriptorSetLayout& descriptor_set_layout) {
    vk::PipelineLayoutCreateInfo pipeline_layout_info;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    return device.createPipelineLayoutUnique(pipeline_layout_info);
}

vk::UniquePipeline create_graphics_pipeline(vk::Device device,
                                            vk::PipelineLayout pipeline_layout,
                                            vk::RenderPass render_pass) {
    vk::UniqueShaderModule vertex_shader(
            shader_module_from_file(device, "shaders/flat.spv"));
    vk::UniqueShaderModule fragment_shader(
            shader_module_from_file(device, "shaders/palette.spv"));

    // Assign vertex shader to its appropriate stage.
    vk::PipelineShaderStageCreateInfo vert_shader_stage_info;
    vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
    vert_shader_stage_info.module = *vertex_shader;
    vert_shader_stage_info.pName = "main";

    // Assign fragment shader as well. Note, in the future, we may want to assign to the
    // pSpecializationInfo field to assign values to constants in the shaders.
    vk::PipelineShaderStageCreateInfo frag_shader_stage_info;
    frag_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
    frag_shader_stage_info.module = *fragment_shader;
    frag_shader_stage_info.pName = "main";

    // Define the two shader stages in an array to later use in the pipeline creation
    // step.
    vk::PipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info,
                                                         frag_shader_stage_info};

    // Get the binding and attribute descriptions for our Vertex struct.
    vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = nullptr;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = nullptr;

    // Use a simple triangle list topology.
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_info;
    input_assembly_state_info.topology = vk::PrimitiveTopology::eTriangleStrip;
    input_assembly_state_info.primitiveRestartEnable = false;

    // Create empty viewport and scissor since these will be dynamic states.
    vk::PipelineViewportStateCreateInfo viewport_state_info;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.pViewports = nullptr;
    viewport_state_info.scissorCount = 1;
    viewport_state_info.pScissors = nullptr;

    // Next, we create the rasterizer, which takes the geometry formed from the vertex
    // shader and turns it into fragments to be colored.
    vk::PipelineRasterizationStateCreateInfo rasterizer_state_info;

    // Clip fragments outside of the depth planes.
    rasterizer_state_info.depthBiasClamp = false;

    // An interesting double negative... Don't discard geometry in the rasterizer stage.
    rasterizer_state_info.rasterizerDiscardEnable = false;

    // We could do point or wireframe here, but we'll do solid triangles for the time
    // being.
    rasterizer_state_info.polygonMode = vk::PolygonMode::eFill;

    // Not all GPUs support thicker lines, so if I ever want to customize this, I must
    // be sure that the GPU does support it.
    rasterizer_state_info.lineWidth = 1.0f;

    // Do no culling since we won't be moving the plane.
    rasterizer_state_info.cullMode = vk::CullModeFlagBits::eNone;
    rasterizer_state_info.frontFace = vk::FrontFace::eCounterClockwise;

    // Disable depth biasing (would only need for shadowing or of the like).
    rasterizer_state_info.depthBiasEnable = false;
    rasterizer_state_info.depthBiasConstantFactor = 0.0f;
    rasterizer_state_info.depthBiasClamp = 0.0f;
    rasterizer_state_info.depthBiasSlopeFactor = 0.0f;

    // Multisampling is one way to perform anti-aliasing, by combining multiple fragment
    // shaders results on a pixel that multiple polygons touch. The effect is most
    // pronounced along edges.
    vk::PipelineMultisampleStateCreateInfo multisampling_info;

    // For now, we disable multisampling.
    multisampling_info.sampleShadingEnable = false;
    multisampling_info.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling_info.minSampleShading = 1.0f;
    multisampling_info.pSampleMask = nullptr;
    multisampling_info.alphaToCoverageEnable = false;
    multisampling_info.alphaToOneEnable = false;

    // TODO(nrg): Depth and stencil testing would be initialized here.

    // Next we define how rendered fragments are blended onto the framebuffer.
    vk::PipelineColorBlendAttachmentState color_blend_attachment;

    // Write all color and alpha channels to framebuffer.
    color_blend_attachment.colorWriteMask =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    // Do not blend fragment into framebuffer. Simply overwrite the framebuffer with the
    // fragment.
    color_blend_attachment.blendEnable = false;

    // (Redundant) Overwrite framebuffer color with input fragment.
    color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eZero;
    color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;

    // (Redundant) Overwrite framebuffer alpha with input fragment.
    color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;

    // Specify constants for fragment blending to framebuffer.
    vk::PipelineColorBlendStateCreateInfo color_blend_info;

    // If true, will enable bitwise blending.
    color_blend_info.logicOpEnable = false;

    // Do simple overwrites rather than blending.
    color_blend_info.logicOp = vk::LogicOp::eCopy;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = &color_blend_attachment;

    // (Redundant) No blending.
    color_blend_info.blendConstants[0] = 0.0f;
    color_blend_info.blendConstants[1] = 0.0f;
    color_blend_info.blendConstants[2] = 0.0f;
    color_blend_info.blendConstants[3] = 0.0f;

    // Allow some dynamic states in our pipeline.
    constexpr static vk::DynamicState dynamic_states[] = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor,
    };
    vk::PipelineDynamicStateCreateInfo dynamic_state_info;
    dynamic_state_info.dynamicStateCount =
            static_cast<uint32_t>(std::size(dynamic_states));
    dynamic_state_info.pDynamicStates = dynamic_states;

    // Finally, we have everything to create the pipeline. We just need to populate it
    // in a struct.
    vk::GraphicsPipelineCreateInfo graphics_pipeline_info;
    graphics_pipeline_info.stageCount = static_cast<uint32_t>(std::size(shader_stages));
    graphics_pipeline_info.pStages = shader_stages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly_state_info;
    graphics_pipeline_info.pViewportState = &viewport_state_info;
    graphics_pipeline_info.pRasterizationState = &rasterizer_state_info;
    graphics_pipeline_info.pMultisampleState = &multisampling_info;
    graphics_pipeline_info.pDepthStencilState = nullptr;
    graphics_pipeline_info.pColorBlendState = &color_blend_info;
    graphics_pipeline_info.pDynamicState = &dynamic_state_info;
    graphics_pipeline_info.layout = pipeline_layout;
    graphics_pipeline_info.renderPass = render_pass;
    graphics_pipeline_info.subpass = 0;
    graphics_pipeline_info.basePipelineHandle = nullptr;
    graphics_pipeline_info.basePipelineIndex = -1;

    return device.createGraphicsPipelineUnique({}, graphics_pipeline_info).value;
}
}  // namespace
}  // namespace maseya::renderer