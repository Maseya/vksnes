#include "LineRenderer.hxx"

namespace maseya::vksnes {
LineRenderer::LineRenderer(VmaAllocator allocator, const Pipeline& line_pipeline,
                           DescriptorSetManager& descriptor_set_manager)
        : pipeline_layout_(line_pipeline.pipeline_layout()),
          pipeline_(*line_pipeline),
          vertex_buffer_(allocator, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         sizeof(glm::vec2) * 0x1000),
          tex_coord_buffer_(allocator, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            sizeof(float) * 0x1000),
          vertex_count_{},
          frame_size_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             sizeof(glm::vec2)),
          line_width_buffer_(allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             sizeof(float)),
          line_descriptor_set_(
                  descriptor_set_manager.allocate_descriptor_set(line_pipeline)) {
    line_descriptor_set_.write(frame_size_buffer_, 0);
    line_descriptor_set_.write(line_width_buffer_, 1);
}

void LineRenderer::set_vertices(const glm::vec2* vertices, const float* tex_coords,
                                size_t size) {
    std::memcpy(vertex_buffer_.data(), vertices, size * sizeof(glm::vec2));
    std::memcpy(tex_coord_buffer_.data(), tex_coords, size * sizeof(float));
    vertex_count_ = static_cast<uint32_t>(size);
}

void LineRenderer::set_line_texture(VkImageView image_view, VkSampler sampler) {
    line_descriptor_set_.write(image_view, sampler, 2);
}

void LineRenderer::queue_draw(const CommandBuffer& command_buffer) const {
    command_buffer.bind_graphics_pipeline(pipeline_);
    command_buffer.bind_descriptor_set(pipeline_layout_, *line_descriptor_set_);

    VkBuffer buffers[2] = {*vertex_buffer_, *tex_coord_buffer_};
    VkDeviceSize offsets[2] = {0, 0};
    vkCmdBindVertexBuffers(*command_buffer, 0,
                           static_cast<uint32_t>(std::size(buffers)), buffers, offsets);

    command_buffer.draw_quads(vertex_count_ / 2);
}
}  // namespace maseya::vksnes