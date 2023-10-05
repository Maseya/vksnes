#pragma once

#include <glm/vec2.hpp>

#include "Buffer.hxx"
#include "CommandBuffer.hxx"
#include "DescriptorSet.hxx"
#include "DescriptorSetManager.hxx"
#include "PipelineManager.hxx"

namespace maseya::vksnes {
class LineRenderer {
public:
    LineRenderer(VmaAllocator allocator, const Pipeline& line_pipeline,
                 DescriptorSetManager& descriptor_set_manager);

    void set_vertices(const std::vector<glm::vec2>& vertices,
                      const std::vector<float>& tex_coords) {
        set_vertices(vertices.data(), tex_coords.data(), vertices.size());
    }

    template <size_t N>
    void set_vertices(const glm::vec2 (&vertices)[N], const float (&tex_coords)[N]) {
        set_vertices(vertices, tex_coords, N);
    }

    void set_vertices(const glm::vec2* vertices, const float* tex_coords, size_t size);

    void set_line_texture(VkImageView image_view, VkSampler sampler);

    VkExtent2D& frame_size() {
        return *static_cast<VkExtent2D*>(frame_size_buffer_.data());
    }
    const VkExtent2D& frame_size() const {
        return *static_cast<const VkExtent2D*>(frame_size_buffer_.data());
    }

    float& line_width() { return *static_cast<float*>(line_width_buffer_.data()); }
    const float& line_width() const {
        return *static_cast<const float*>(line_width_buffer_.data());
    }

    void queue_draw(const CommandBuffer& command_buffer) const;

private:
    VkPipelineLayout pipeline_layout_;
    VkPipeline pipeline_;
    Buffer vertex_buffer_;
    Buffer tex_coord_buffer_;
    uint32_t vertex_count_;
    Buffer frame_size_buffer_;
    Buffer line_width_buffer_;
    DescriptorSet line_descriptor_set_;
};
}  // namespace maseya::vksnes
