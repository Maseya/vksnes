#version 450
#extension GL_KHR_vulkan_glsl : enable

#define VERSION_DEFINED

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform Stride {
    uint width;
} stride;

layout(binding = 1) readonly buffer Palette {
    uint[0x80] colors;
} palette;

layout(binding = 2) readonly buffer Gfx {
    uint[0x4000] pixels;
} gfx;

layout(binding = 3) readonly buffer ObjBuffer {
#ifndef WORD_INDEX
    uint[0x80] tiles;
#else
    uint[0x8000] tiles;
#endif
} obj;

layout(binding = 4) readonly buffer MapBuffer {
#ifndef WORD_INDEX
    uint[0x10000] tiles;
#else
    uint[0x8000] tiles;
#endif
    uint size;
} map;

#include "get_palette_color.glsl"
#include "gfx_formats.glsl"
#ifndef GFX_FORMAT
#define GFX_FORMAT FORMAT_4BPP_SNES
#endif

#include "get_color_index.glsl"
#include "get_obj_tile.glsl"

#ifdef MAP16_TILEMAP
#define TILE_SIZE 16
#endif
#include "get_tile_index.glsl"

#ifdef MAP16_TILEMAP
uint get_map8_tile_index(vec2 pixel);
#endif

uint get_map_tile(uint index);

void main() {
    uint tile_index = get_tile_index(gl_FragCoord.xy - 0.5, stride.width);
    if (tile_index >= map.size / 2)
        discard;

    uvec2 tile_pixel = get_tile_pixel(gl_FragCoord.xy - 0.5);

    uint map_tile = get_map_tile(tile_index);
#ifdef MAP16_TILEMAP
    map_tile += get_map8_tile_index(gl_FragCoord.xy - 0.5);
#endif

    uint obj_tile = get_obj_tile(map_tile);
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
uint get_map8_tile_index(vec2 pixel) {
    uint x = (uint(pixel.x) & 8) >> 2;
    uint y = (uint(pixel.y) & 8) >> 3;

    return x | y;
}
#endif

uint get_map_tile(uint index) {
#ifndef WORD_INDEX
    uint array_index = index >> 2;
    uint bit_shift = (index & 3) << 3;
    return ((map.tiles[array_index] >> bit_shift) & 0xff) << 2;
#else
    uint array_index = index >> 1;
    uint bit_shift = (index & 1) << 4;
    return ((obj.tiles[array_index] >> bit_shift) & 0xffff) << 2;
#endif
}
