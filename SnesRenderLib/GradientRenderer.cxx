#include "GradientRenderer.hxx"

namespace maseya::vksnes {
GradientRenderer::GradientRenderer(VmaAllocator allocator,
                                   const Pipeline& gradient_pipeline,
                                   DescriptorSetManager& descriptor_set_manager)
        : gradient_pipeline_(*gradient_pipeline),
          gradient_pipeline_layout_(gradient_pipeline.pipeline_layout()),
          frame_size_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             sizeof(VkExtent2D)),
          gradient_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           sizeof(GradientPattern)),
          gradient_descriptor_set_(
                  descriptor_set_manager.allocate_descriptor_set(gradient_pipeline)) {
    gradient_descriptor_set_.write(frame_size_buffer_, 0);
    gradient_descriptor_set_.write(gradient_buffer_, 1);
}

void GradientRenderer::queue_draw(const CommandBuffer& command_buffer) const {
    command_buffer.bind_graphics_pipeline(gradient_pipeline_);
    command_buffer.bind_descriptor_set(gradient_pipeline_layout_,
                                       *gradient_descriptor_set_);
    command_buffer.draw_quads(1);
}
}  // namespace maseya::vksnes