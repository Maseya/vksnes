#include "ObjRenderer.hxx"

namespace maseya::vksnes {
ObjRenderer::ObjRenderer(VmaAllocator allocator,
                         const PipelineManager& pipeline_manager,
                         DescriptorSetManager& descriptor_set_manager)
        : pipeline_manager_(&pipeline_manager),
          map16_(true),
          format_(PixelFormat::Format4BppSnes),
          stride_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         sizeof(uint32_t)),
          priority_mask_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                sizeof(uint32_t)),
          obj_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(ObjData)),
          palette_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                          0x100 * sizeof(uint16_t)),
          gfx_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 0x10000),
          obj_descriptor_set_(descriptor_set_manager.allocate_descriptor_set(
                  pipeline_manager_->obj_pipeline(map16_, format_))) {
    obj_descriptor_set_.write(stride_buffer_, 0);
    obj_descriptor_set_.write(priority_mask_buffer_, 1);
    obj_descriptor_set_.write(palette_buffer_, 2);
    obj_descriptor_set_.write(gfx_buffer_, 3);
    obj_descriptor_set_.write(obj_buffer_, 4);
}

void ObjRenderer::set_palette(const uint16_t* palette, uint32_t size) {
    std::memcpy(palette_buffer_.data(), palette, size * sizeof(uint16_t));
}

void ObjRenderer::set_gfx(const uint8_t* gfx, uint32_t size) {
    std::memcpy(gfx_buffer_.data(), gfx, size);
}

void ObjRenderer::set_obj_buffer(const uint16_t* obj, uint32_t size) {
    std::memcpy(obj_data().data, obj, size * sizeof(uint16_t));
    obj_data().size = size;
}

void ObjRenderer::queue_draw(const CommandBuffer& command_buffer) const {
    command_buffer.bind_graphics_pipeline(
            *pipeline_manager_->obj_pipeline(map16_, format_));
    command_buffer.bind_descriptor_set(
            pipeline_manager_->obj_pipeline(map16_, format_).pipeline_layout(),
            *obj_descriptor_set_);
    command_buffer.draw_quads(obj_data().size);
}
}  // namespace maseya::vksnes