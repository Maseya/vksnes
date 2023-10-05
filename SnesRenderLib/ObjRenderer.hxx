#pragma once

#include "Buffer.hxx"
#include "CommandBuffer.hxx"
#include "DescriptorSet.hxx"
#include "DescriptorSetManager.hxx"
#include "Device.hxx"
#include "ObjData.hxx"
#include "PipelineManager.hxx"
#include "PixelFormat.hxx"

namespace maseya::vksnes {
class ObjRenderer {
public:
    ObjRenderer(VmaAllocator allocator, const PipelineManager& pipeline_manager,
                DescriptorSetManager& descriptor_set_manager);

    bool& map16() { return map16_; }
    bool map16() const { return map16_; }

    PixelFormat& format() { return format_; }
    const PixelFormat& format() const { return format_; }

    uint32_t& stride() { return *static_cast<uint32_t*>(stride_buffer_.data()); }
    const uint32_t& stride() const {
        return *static_cast<const uint32_t*>(stride_buffer_.data());
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

    ObjData& obj_data() { return *static_cast<ObjData*>(obj_buffer_.data()); }
    const ObjData& obj_data() const {
        return *static_cast<const ObjData*>(obj_buffer_.data());
    }

    void set_obj_buffer(const uint16_t* obj, uint32_t size);

    void set_obj_buffer(const std::vector<uint16_t>& obj) {
        set_obj_buffer(obj.data(), static_cast<uint32_t>(obj.size()));
    }

    template <size_t N>
    void set_obj_buffer(const uint16_t (&obj)[N]) {
        set_obj_buffer(obj, static_cast<uint32_t>(N));
    }

    void queue_draw(const CommandBuffer& command_buffer) const;

private:
    const PipelineManager* pipeline_manager_;
    bool map16_;
    PixelFormat format_;
    Buffer stride_buffer_;
    Buffer priority_mask_buffer_;
    Buffer obj_buffer_;
    Buffer palette_buffer_;
    Buffer gfx_buffer_;
    DescriptorSet obj_descriptor_set_;
};
}  // namespace maseya::vksnes
