#pragma once

#include "Buffer.hxx"
#include "CommandBuffer.hxx"
#include "DescriptorSet.hxx"
#include "DescriptorSetManager.hxx"
#include "Device.hxx"
#include "Map16Data.hxx"
#include "ObjData.hxx"
#include "PipelineManager.hxx"
#include "PixelFormat.hxx"

namespace maseya::vksnes {
class Map16Renderer {
public:
    Map16Renderer(VmaAllocator allocator, const PipelineManager& pipeline_manager,
                  DescriptorSetManager& descriptor_set_manager);

    bool& map16() { return map16_; }
    bool map16() const { return map16_; }

    PixelFormat& format() { return format_; }
    const PixelFormat& format() const { return format_; }

    uint32_t& stride() { return *static_cast<uint32_t*>(stride_buffer_.data()); }
    const uint32_t& stride() const {
        return *static_cast<const uint32_t*>(stride_buffer_.data());
    }

    void set_palette(const uint16_t* palette, uint32_t size) {
        std::memcpy(palette_buffer_.data(), palette, size * sizeof(uint16_t));
    }

    void set_palette(const std::vector<uint16_t>& palette) {
        set_palette(palette.data(), static_cast<uint32_t>(palette.size()));
    }

    template <size_t N>
    void set_palette(const uint16_t (&palette)[N]) {
        set_palette(palette, static_cast<uint32_t>(N));
    }

    void set_gfx(const uint8_t* gfx, uint32_t size) {
        std::memcpy(gfx_buffer_.data(), gfx, size);
    }

    void set_gfx(const std::vector<uint8_t>& gfx) {
        set_gfx(gfx.data(), static_cast<uint32_t>(gfx.size()));
    }

    template <size_t N>
    void set_gfx(const uint8_t (&gfx)[N]) {
        set_gfx(gfx, static_cast<uint32_t>(N));
    }

    void set_obj_buffer(const uint16_t* obj, uint32_t size) {
        std::memcpy(obj_buffer_.data(), obj, size * sizeof(uint16_t));
    }

    void set_obj_buffer(const std::vector<uint16_t>& obj) {
        set_obj_buffer(obj.data(), static_cast<uint32_t>(obj.size()));
    }

    template <size_t N>
    void set_obj_buffer(const uint16_t (&obj)[N]) {
        set_obj_buffer(obj, static_cast<uint32_t>(N));
    }

    Map16Data& map16_data() { return *static_cast<Map16Data*>(map16_buffer_.data()); }
    const Map16Data& map16_data() const {
        return *static_cast<const Map16Data*>(map16_buffer_.data());
    }

    void set_map16_buffer(const uint8_t* map16, uint32_t size) {
        std::memcpy(map16_data().data, map16, size);
        map16_data().size = size;
    }

    void set_map16_buffer(const std::vector<uint8_t>& map16) {
        set_map16_buffer(map16.data(), static_cast<uint32_t>(map16.size()));
    }

    template <size_t N>
    void set_map16_buffer(const uint8_t (&map16)[N]) {
        set_map16_buffer(map16, static_cast<uint32_t>(N));
    }

    void queue_draw(const CommandBuffer& command_buffer) const;

private:
    const PipelineManager* pipeline_manager_;
    bool map16_;
    PixelFormat format_;
    Buffer stride_buffer_;
    Buffer map16_buffer_;
    Buffer palette_buffer_;
    Buffer gfx_buffer_;
    Buffer obj_buffer_;
    DescriptorSet map16_descriptor_set_;
};
}  // namespace maseya::vksnes
