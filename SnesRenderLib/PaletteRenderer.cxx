#include "PaletteRenderer.hxx"

namespace maseya::vksnes {
PaletteRenderer::PaletteRenderer(VmaAllocator allocator,
                                 const Pipeline& palette_pipeline,
                                 DescriptorSetManager& descriptor_set_manager)
        : palette_pipeline_(*palette_pipeline),
          palette_pipeline_layout_(palette_pipeline.pipeline_layout()),
          stride_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         sizeof(uint32_t)),
          palette_data_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                               sizeof(PaletteData)),
          palette_descriptor_set_(
                  descriptor_set_manager.allocate_descriptor_set(palette_pipeline)) {
    palette_descriptor_set_.write(stride_buffer_, 0);
    palette_descriptor_set_.write(palette_data_buffer_, 1);
}

void PaletteRenderer::set_palette_buffer(const uint16_t* palette, uint32_t size) {
    std::memcpy(palette_data().data, palette, size * sizeof(uint16_t));
    palette_data().size = size;
}

void PaletteRenderer::queue_draw(const CommandBuffer& command_buffer) const {
    command_buffer.bind_graphics_pipeline(palette_pipeline_);
    command_buffer.bind_descriptor_set(palette_pipeline_layout_,
                                       *palette_descriptor_set_);
    command_buffer.draw_quads(palette_data().size);
}
}  // namespace maseya::vksnes