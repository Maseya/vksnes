#include "GfxRenderer.hxx"

namespace maseya::vksnes {
GfxRenderer::GfxRenderer(VmaAllocator allocator,
                         const PipelineManager& pipeline_manager,
                         DescriptorSetManager& descriptor_set_manager)
        : pipeline_manager_(&pipeline_manager),
          format_(PixelFormat::Format4BppSnes),
          stride_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         sizeof(uint32_t)),
          palette_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                          0x100 * sizeof(uint16_t)),
          gfx_data_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                           sizeof(GfxData)),
          gfx_descriptor_set_(descriptor_set_manager.allocate_descriptor_set(
                  pipeline_manager_->gfx_pipeline(format_))) {
    gfx_descriptor_set_.write(stride_buffer_, 0);
    gfx_descriptor_set_.write(palette_buffer_, 1);
    gfx_descriptor_set_.write(gfx_data_buffer_, 2);
}

void GfxRenderer::set_palette(const uint16_t* palette, uint32_t size) {
    std::memcpy(palette_buffer_.data(), palette, size * sizeof(uint16_t));
}

void GfxRenderer::set_gfx_buffer(const uint8_t* gfx, uint32_t size) {
    std::memcpy(gfx_data().data, gfx, size);
    gfx_data().size = size;
}

void GfxRenderer::queue_draw(const CommandBuffer& command_buffer) const {
    command_buffer.bind_graphics_pipeline(*pipeline_manager_->gfx_pipeline(format_));
    command_buffer.bind_descriptor_set(
            pipeline_manager_->gfx_pipeline(format_).pipeline_layout(),
            *gfx_descriptor_set_);

    uint32_t bytes_per_plane = static_cast<uint32_t>(format_) & 0x0F;
    uint32_t bytes_per_tile = bytes_per_plane * 8;
    command_buffer.draw_quads(gfx_data().size / bytes_per_tile);
}
}  // namespace maseya::vksnes