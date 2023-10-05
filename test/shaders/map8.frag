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
#ifdef MAP8_TILES
    uint[0x80] tiles;
#else
    uint[0x8000] tiles;
#endif
} obj;

layout(binding = 4) readonly buffer MapBuffer {
#ifdef BYTE_INDEX
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

uint get_tile_index(vec2 pixel, uint stride);

#ifdef MAP16_TILEMAP
uint get_map16_tile_index(vec2 pixel);
#endif

uint get_map_tile(uint index);

uint get_obj_tile(uint index);

uvec2 get_tile_pixel(vec2 coord);

void main() {
    uint tile_index = get_tile_index(gl_FragCoord.xy - 0.5, stride.width);
    if (tile_index >= map.size / 2)
        discard;

    uint map_tile = get_map_tile(tile_index);
#ifdef MAP16_TILEMAP
    map_tile += get_map16_tile_index(gl_FragCoord.xy - 0.5);
#endif

    uint obj_tile = get_obj_tile(map_tile);
    uint gfx_tile_index = obj_tile & 0x3FF;
    uint palette_offset = ((obj_tile >> 10) & 7) << 4;
    uint flip_mask = (obj_tile >> 14) & 3;
    uvec2 tile_pixel = get_tile_pixel(gl_FragCoord.xy - 0.5);
    tile_pixel.x ^= (flip_mask & 1) * 7;
    tile_pixel.y ^= ((flip_mask & 2) >> 1) * 7;
    uint color_index = get_color_index(gfx_tile_index, tile_pixel);

    uint palette_color = get_palette_color(color_index + palette_offset);

    FragColor = vec4(get_rgb_color(palette_color), color_index > 0 ? 1 : 0);
}

uint get_tile_index(vec2 pixel, uint stride) {
#ifdef MAP16_TILEMAP
    uint x = uint(pixel.x / 16);
    uint y = uint(pixel.y / 16);
#else
    uint x = uint(pixel.x / 8);
    uint y = uint(pixel.y / 8);
#endif
    return (y * stride) + x;
}

#ifdef MAP16_TILEMAP
uint get_map16_tile_index(vec2 pixel) {
    uint x = (uint(pixel.x) & 8) >> 2;
    uint y = (uint(pixel.y) & 8) >> 3;

    return x | y;
}
#endif

uint get_map_tile(uint index) {
#ifdef BYTE_INDEX
    uint array_index = index >> 2;
    uint bit_shift = (index & 3) << 2;
    return (map.tiles[array_index] >> bit_shift) & 0xff;
#else
    uint array_index = index >> 1;
    uint bit_shift = (index & 1) << 4;
    return ((obj.tiles[array_index] >> bit_shift) & 0xffff) << 2;
#endif
}

uint get_obj_tile(uint index) {
    // Get the actual index into the palette.colors array. We are packing the palette's
    // 16-bit colors into a 32-bit array, so two colors actually exist per array
    // element.
    uint array_index = index >> 1;

    // Let's us get either the high or low word of that array element.
    uint bit_shift = (index & 1) << 4;

    // Don't forget to reduce the result to 16 bits.
    return (obj.tiles[array_index] >> bit_shift) & 0xffff;
}

uvec2 get_tile_pixel(vec2 coord) {
    return uvec2(coord) & 7;
}
