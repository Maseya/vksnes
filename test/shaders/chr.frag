#version 450
#extension GL_KHR_vulkan_glsl : enable
#define VERSION_DEFINED

layout (location = 0) flat in uvec2 Pixel;
layout (location = 1) flat in uint ChrTile;

layout(location = 0) out vec4 FragColor;

layout(binding = 1) uniform Priority {
    uint mask;
} priority;

layout(binding = 2) readonly buffer Palette {
    uint[0x80] colors;
} palette;

layout(binding = 3) readonly buffer Gfx {
    uint[0x4000] pixels;
} gfx;

#include "get_palette_color.glsl"
#include "gfx_formats.glsl"
#ifndef GFX_FORMAT
#define GFX_FORMAT FORMAT_4BPP_SNES
#endif

#include "get_color_index.glsl"

void main() {
    uint chr_tile = ChrTile & 0xFFFFu;
    uint tile_priority_bit = 1 << ((chr_tile >> 12) & 3);
    if ((tile_priority_bit & priority.mask) == 0) {
        discard;
    }

    uint gfx_tile_index = chr_tile & 0x1FF;
    uint flip_mask = (chr_tile >> 14) & 3;
    uvec2 tile_pixel = uvec2(gl_FragCoord.xy - 0.5) - Pixel;
    tile_pixel.x ^= (flip_mask & 1) * 7;
    tile_pixel.y ^= ((flip_mask & 2) >> 1) * 7;
    uint color_index = get_color_index(gfx_tile_index, tile_pixel);
    if (color_index == 0) {
        discard;
    }

    uint palette_offset = ((chr_tile >> 9) & 7) << 4;
    uint palette_color = get_palette_color(color_index + palette_offset);

    FragColor = vec4(get_rgb_color(palette_color), 1);
}
