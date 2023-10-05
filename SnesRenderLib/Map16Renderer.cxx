#include "Map16Renderer.hxx"

namespace maseya::vksnes {
Map16Renderer::Map16Renderer(VmaAllocator allocator,
                             const PipelineManager& pipeline_manager,
                             DescriptorSetManager& descriptor_set_manager)
        : pipeline_manager_(&pipeline_manager),
          map16_(true),
          format_(PixelFormat::Format4BppSnes),
          stride_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         sizeof(uint32_t)),
          obj_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 0x800),
          map16_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                        sizeof(Map16Data)),
          palette_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                          0x100 * sizeof(uint16_t)),
          gfx_buffer_(allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 0x10000),
          map16_descriptor_set_(descriptor_set_manager.allocate_descriptor_set(
                  pipeline_manager_->map_pipeline(map16_, true, format_))) {
    map16_descriptor_set_.write(stride_buffer_, 0);
    map16_descriptor_set_.write(palette_buffer_, 1);
    map16_descriptor_set_.write(gfx_buffer_, 2);
    map16_descriptor_set_.write(obj_buffer_, 3);
    map16_descriptor_set_.write(map16_buffer_, 4);
}

void Map16Renderer::queue_draw(const CommandBuffer& command_buffer) const {
    command_buffer.bind_graphics_pipeline(
            *pipeline_manager_->map_pipeline(map16_, true, format_));
    command_buffer.bind_descriptor_set(
            pipeline_manager_->map_pipeline(map16_, true, format_).pipeline_layout(),
            *map16_descriptor_set_);
    command_buffer.draw(3, 1);
}
}  // namespace maseya::vksnes