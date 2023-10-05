#pragma once

#include "Buffer.hxx"
#include "CommandBuffer.hxx"
#include "DescriptorSet.hxx"
#include "DescriptorSetManager.hxx"
#include "Device.hxx"
#include "PipelineManager.hxx"
#include "PixelFormat.hxx"

namespace maseya::vksnes {
class ChrRenderer {
public:
    ChrRenderer(VmaAllocator allocator, const PipelineManager& pipeline_manager,
                DescriptorSetManager& descriptor_set_manager);

    PixelFormat& format() { return format_; }
    const PixelFormat& format() const { return format_; }

    void set_chr_tiles(const uint32_t* chr_tiles, const VkExtent2D* coords,
                       size_t size);

    void set_chr_tiles(const std::vector<uint32_t>& chr_tiles,
                       const std::vector<VkExtent2D>& coords) {
        set_chr_tiles(chr_tiles.data(), coords.data(), chr_tiles.size());
    }

    template <size_t N>
    void set_chr_tiles(const uint32_t (&chr_tiles)[N], const VkExtent2D (&coords)[N]) {
        set_chr_tiles(chr_tiles, coords, N);
    }

    VkExtent2D& frame() { return *static_cast<VkExtent2D*>(frame_buffer_.data()); }
    const VkExtent2D& frame() const {
        return *static_cast<const VkExtent2D*>(frame_buffer_.data());
    }

    uint32_t& priority_mask() {
        return *static_cast<uint32_t*>(priority_mask_buffer_.data());
    }
    const uint32_t& priority_mask() const {
        return *static_cast<const uint32_t*>(priority_mask_buffer_.data());
    }

    void set_palette(const uint16_t* palette, uint32_t size);

    void set_palette(const std::vector<uint16_t>& palette) {
        set_palette(palette.data(), static_cast<uint32_t>(palette.size()));
    }

    template <size_t N>
    void set_palette(const uint16_t (&palette)[N]) {
        set_palette(palette, static_cast<uint32_t>(N));
    }

    void set_gfx(const uint8_t* gfx, uint32_t size);

    void set_gfx(const std::vector<uint8_t>& gfx) {
        set_gfx(gfx.data(), static_cast<uint32_t>(gfx.size()));
    }

    template <size_t N>
    void set_gfx(const uint8_t (&gfx)[N]) {
        set_gfx(gfx, static_cast<uint32_t>(N));
    }

    void queue_draw(const CommandBuffer& command_buffer) const;

private:
    const PipelineManager* pipeline_manager_;
    bool map16_;
    PixelFormat format_;
    Buffer chr_tile_buffer_;
    Buffer coord_buffer_;
    uint32_t vertex_count_;
    Buffer frame_buffer_;
    Buffer priority_mask_buffer_;
    Buffer palette_buffer_;
    Buffer gfx_buffer_;
    DescriptorSet chr_descriptor_set_;
};
}  // namespace maseya::vksnes
