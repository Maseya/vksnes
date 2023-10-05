#version 450
#extension GL_KHR_vulkan_glsl : enable
#define VERSION_DEFINED

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform Stride {
    uint width;
} stride;

layout(binding = 1) uniform Priority {
    uint mask;
} priority;

layout(binding = 2) readonly buffer Palette {
    uint[0x80] colors;
} palette;

layout(binding = 3) readonly buffer Gfx {
    uint[0x4000] pixels;
} gfx;

layout(binding = 4) readonly buffer ObjBuffer {
    uint[0x8000] tiles;
    uint size;
} obj;

#include "get_palette_color.glsl"
#include "gfx_formats.glsl"
#ifndef GFX_FORMAT
#define GFX_FORMAT FORMAT_4BPP_SNES
#endif

#include "get_color_index.glsl"
#include "get_obj_tile.glsl"
#include "get_tile_index.glsl"

#ifdef MAP16_TILEMAP
uint get_map16_tile_index(uint index, uint stride);
#endif

void main() {
    uint tile_index = get_tile_index(gl_FragCoord.xy - 0.5, stride.width);
#ifdef MAP16_TILEMAP
    tile_index = get_map16_tile_index(tile_index, stride.width);
#endif
    if (tile_index >= obj.size / 2)
        discard;

    uvec2 tile_pixel = get_tile_pixel(gl_FragCoord.xy - 0.5);

    uint obj_tile = get_obj_tile(tile_index);
    uint tile_priority_bit = 1 << ((obj_tile >> 13) & 1);
    if ((tile_priority_bit & priority.mask) == 0) {
        discard;
    }

    uint gfx_tile_index = obj_tile & 0x3FF;
    uint flip_mask = (obj_tile >> 14) & 3;
    tile_pixel.x ^= (flip_mask & 1) * 7;
    tile_pixel.y ^= ((flip_mask & 2) >> 1) * 7;
    uint color_index = get_color_index(gfx_tile_index, tile_pixel);
    if (color_index == 0) {
        discard;
    }

    uint palette_offset = ((obj_tile >> 10) & 7) << 4;
    uint palette_color = get_palette_color(color_index + palette_offset);

    FragColor = vec4(get_rgb_color(palette_color), color_index > 0 ? 1 : 0);
}

#ifdef MAP16_TILEMAP
uint get_map16_tile_index(uint index, uint stride) {
    uint relative_index = index % (stride << 1);

    index += relative_index;
    if (relative_index >= stride) {
        index += 1 - (stride << 1);
    }

    return index;
}
#endif
