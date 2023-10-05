#include "ChrRenderer.hxx"

namespace maseya::vksnes {
ChrRenderer::ChrRenderer(VmaAllocator allocator,
                         const PipelineManager& pipeline_manager,
                         DescriptorSetManager& descriptor_set_manager)
        : pipeline_manager_(&pipeline_manager),
          map16_(true),
          format_(PixelFormat::Format4BppSnes),
          chr_tile_buffer_(allocator, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                           sizeof(uint32_t) * 0x4000),
          coord_buffer_(allocator, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        sizeof(VkExtent2D) * 0x4000),
          frame_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        2 * sizeof(uint32_t)),
          priority_mask_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                sizeof(uint32_t)),
          palette_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                          0x100 * sizeof(uint16_t)),
          gfx_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 0x10000),
          chr_descriptor_set_(descriptor_set_manager.allocate_descriptor_set(
                  pipeline_manager_->chr_pipeline(format_))) {
    chr_descriptor_set_.write(frame_buffer_, 0);
    chr_descriptor_set_.write(priority_mask_buffer_, 1);
    chr_descriptor_set_.write(palette_buffer_, 2);
    chr_descriptor_set_.write(gfx_buffer_, 3);
}

void ChrRenderer::set_chr_tiles(const uint32_t* chr_tiles, const VkExtent2D* coords,
                                size_t size) {
    std::memcpy(chr_tile_buffer_.data(), chr_tiles, size * sizeof(uint32_t));
    std::memcpy(coord_buffer_.data(), coords, size * sizeof(VkExtent2D));
    vertex_count_ = static_cast<uint32_t>(size);
}

void ChrRenderer::set_palette(const uint16_t* palette, uint32_t size) {
    std::memcpy(palette_buffer_.data(), palette, size * sizeof(uint16_t));
}

void ChrRenderer::set_gfx(const uint8_t* gfx, uint32_t size) {
    std::memcpy(gfx_buffer_.data(), gfx, size);
}

void ChrRenderer::queue_draw(const CommandBuffer& command_buffer) const {
    command_buffer.bind_graphics_pipeline(*pipeline_manager_->chr_pipeline(format_));
    command_buffer.bind_descriptor_set(
            pipeline_manager_->chr_pipeline(format_).pipeline_layout(),
            *chr_descriptor_set_);

    VkBuffer buffers[2] = {*coord_buffer_, *chr_tile_buffer_};
    VkDeviceSize offsets[2] = {0, 0};
    vkCmdBindVertexBuffers(*command_buffer, 0,
                           static_cast<uint32_t>(std::size(buffers)), buffers, offsets);

    command_buffer.draw_quads(vertex_count_);
}
}  // namespace maseya::vksnes