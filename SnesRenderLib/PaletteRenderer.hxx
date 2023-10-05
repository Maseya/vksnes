#pragma once

#include "Buffer.hxx"
#include "CommandBuffer.hxx"
#include "DescriptorSet.hxx"
#include "DescriptorSetManager.hxx"
#include "PaletteData.hxx"
#include "Pipeline.hxx"

namespace maseya::vksnes {
class PaletteRenderer {
public:
    PaletteRenderer(VmaAllocator allocator, const Pipeline& palette_pipeline,
                    DescriptorSetManager& descriptor_set_manager);

    uint32_t& stride() { return *static_cast<uint32_t*>(stride_buffer_.data()); }
    const uint32_t& stride() const {
        return *static_cast<const uint32_t*>(stride_buffer_.data());
    }

    PaletteData& palette_data() {
        return *static_cast<PaletteData*>(palette_data_buffer_.data());
    }
    const PaletteData& palette_data() const {
        return *static_cast<const PaletteData*>(palette_data_buffer_.data());
    }

    void set_palette_buffer(const uint16_t* palette, uint32_t size);

    void set_palette_buffer(const std::vector<uint16_t>& palette) {
        set_palette_buffer(palette.data(), static_cast<uint32_t>(palette.size()));
    }

    template <size_t N>
    void set_palette_buffer(const uint16_t (&palette)[N]) {
        set_palette_buffer(palette, static_cast<uint32_t>(N));
    }

    void queue_draw(const CommandBuffer& command_buffer) const;

private:
    VkPipeline palette_pipeline_;
    VkPipelineLayout palette_pipeline_layout_;
    Buffer stride_buffer_;
    Buffer palette_data_buffer_;
    DescriptorSet palette_descriptor_set_;
};
}  // namespace maseya::vksnes
