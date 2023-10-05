#include "OverlayRenderer.hxx"

namespace maseya::vksnes {
OverlayRenderer::OverlayRenderer(VkDevice device, const Pipeline& overlay_pipeline,
                                 DescriptorSetManager& descriptor_set_manager)
        : device_(device),
          overlay_pipeline_(*overlay_pipeline),
          overlay_pipeline_layout_(overlay_pipeline.pipeline_layout()),
          overlay_descriptor_set_(
                  descriptor_set_manager.allocate_descriptor_set(overlay_pipeline)) {}

void OverlayRenderer::set_image_view(VkImageView image_view, VkSampler sampler) {
    overlay_descriptor_set_.write(image_view, sampler, 0);
}

void OverlayRenderer::queue_draw(const CommandBuffer& command_buffer) const {
    command_buffer.bind_graphics_pipeline(overlay_pipeline_);
    command_buffer.bind_descriptor_set(overlay_pipeline_layout_,
                                       *overlay_descriptor_set_);
    command_buffer.draw(3, 1);
}
}  // namespace maseya::vksnes