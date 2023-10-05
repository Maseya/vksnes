#pragma once

#include "Buffer.hxx"
#include "CommandBuffer.hxx"
#include "DescriptorSet.hxx"
#include "DescriptorSetManager.hxx"
#include "Device.hxx"
#include "GfxData.hxx"
#include "PipelineManager.hxx"
#include "PixelFormat.hxx"

namespace maseya::vksnes {
class GfxRenderer {
public:
    GfxRenderer(VmaAllocator allocator, const PipelineManager& pipeline_manager,
                DescriptorSetManager& descriptor_set_manager);

    PixelFormat& format() { return format_; }
    const PixelFormat& format() const { return format_; }

    uint32_t& stride() { return *static_cast<uint32_t*>(stride_buffer_.data()); }
    const uint32_t& stride() const {
        return *static_cast<const uint32_t*>(stride_buffer_.data());
    }

    void set_palette(const uint16_t* palette, uint32_t size);
    void set_palette(const std::vector<uint16_t>& palette) {
        set_palette(palette.data(), static_cast<uint32_t>(palette.size()));
    }
    template <size_t N>
    void set_palette(const uint16_t (&palette)[N]) {
        std::memcpy(palette_buffer_.data(), palette, static_cast<uint32_t>(N));
    }

    GfxData& gfx_data() { return *static_cast<GfxData*>(gfx_data_buffer_.data()); }
    const GfxData& gfx_data() const {
        return *static_cast<const GfxData*>(gfx_data_buffer_.data());
    }

    void set_gfx_buffer(const uint8_t* gfx, uint32_t size);
    void set_gfx_buffer(const std::vector<uint8_t>& gfx) {
        set_gfx_buffer(gfx.data(), static_cast<uint32_t>(gfx.size()));
    }
    template <size_t N>
    void set_gfx_buffer(const uint8_t (&gfx)[N]) {
        set_gfx_buffer(gfx, static_cast<uint32_t>(N));
    }

    void queue_draw(const CommandBuffer& command_buffer) const;

private:
    const PipelineManager* pipeline_manager_;
    PixelFormat format_;
    Buffer stride_buffer_;
    Buffer palette_buffer_;
    Buffer gfx_data_buffer_;
    DescriptorSet gfx_descriptor_set_;
};
}  // namespace maseya::vksnes
