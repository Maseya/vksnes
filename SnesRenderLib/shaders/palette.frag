// palette.frag
// Copyright (C) 2022 Nelson Garcia
//
// This source is public domain.

#version 450
#extension GL_KHR_vulkan_glsl : enable

#define VERSION_DEFINED

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform Stride {
    uint width;
} stride;

layout(binding = 1) readonly buffer PaletteBuffer {
// 16-bit integers are not guaranteed to be usable on every device, so we encode the
// palette data in a 32-bit uint array. Each element packs 2 colors per 16 bits.
uint[0x8000] colors;

// Get the number of colors actually written to the buffer. While the buffer has a fixed
// size, it is allowed to use less color than the max.
uint size;
} palette;

#include "get_palette_color.glsl"

uint get_tile_index(vec2 pixel, uint stride);

void main() {
    uint palette_index = get_tile_index(gl_FragCoord.xy - 0.5, stride.width);

    // Don't draw anything for pixels not in the actual palette.
    if (palette_index >= palette.size) discard;

    uint palette_color = get_palette_color(palette_index);

    // Note that sometimes we can specify transparency (or just black) for the first
    // color in a row, but this isn't great to do for generic palettes. Some will use
    // special first colors to help you distinguish background from actual black, and
    // some palettes are only 15 colors per row, with an implicit transparent color.
    FragColor = vec4(get_rgb_color(palette_color), 1);
}

uint get_tile_index(vec2 pixel, uint stride) {
    uint x = uint(pixel.x / 8);
    uint y = uint(pixel.y / 8);
    return (y * stride) + x;
}