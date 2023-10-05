#include "CheckerboardRenderer.hxx"

namespace maseya::vksnes {
CheckerboardRenderer::CheckerboardRenderer(VmaAllocator allocator,
                                           const Pipeline& checker_pipeline,
                                           DescriptorSetManager& descriptor_set_manager)
        : checker_pipeline_(*checker_pipeline),
          checker_pipeline_layout_(checker_pipeline.pipeline_layout()),
          checker_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                          sizeof(CheckerboardPattern)),
          checker_descriptor_set_(
                  descriptor_set_manager.allocate_descriptor_set(checker_pipeline)) {
    checker_descriptor_set_.write(checker_buffer_, 0);
}

void CheckerboardRenderer::queue_draw(const CommandBuffer& command_buffer) const {
    command_buffer.bind_graphics_pipeline(checker_pipeline_);
    command_buffer.bind_descriptor_set(checker_pipeline_layout_,
                                       *checker_descriptor_set_);
    command_buffer.draw_quads(1);
}
}  // namespace maseya::vksnes