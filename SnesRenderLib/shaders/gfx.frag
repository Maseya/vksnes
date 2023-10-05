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
    uint[0x8000] pixels;
    uint size;
} gfx;

#include "get_palette_color.glsl"
#include "gfx_formats.glsl"
#ifndef GFX_FORMAT
#define GFX_FORMAT FORMAT_4BPP_SNES
#endif

#include "get_color_index.glsl"

#define BPP_SIZE(format) ((format) & 0x0F)

// Get the tile index of the tile at the given pixel.
uint get_tile_index(vec2 pixel, uint stride);

void main() {
    uint tile_index = get_tile_index(gl_FragCoord.xy - 0.5, stride.width);
    if (tile_index >= gfx.size / (BPP_SIZE(GFX_FORMAT) * 8))
        discard;

    uvec2 tile_pixel = get_tile_pixel(gl_FragCoord.xy - 0.5);

    uint color_index = get_color_index(tile_index, tile_pixel);
    uint palette_color = get_palette_color(color_index);
    FragColor = vec4(get_rgb_color(palette_color), 1);
}

uint get_tile_index(vec2 pixel, uint stride) {
    uint x = uint(pixel.x / 8);
    uint y = uint(pixel.y / 8);
    return (y * stride) + x;
}

