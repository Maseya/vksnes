#ifndef GET_COLOR_INDEX_GLSL
#define GET_COLOR_INDEX_GLSL

#ifndef VERSION_DEFINED
#extension GL_EXT_gpu_shader4 : enable

// Dummy struct to remove Intellisense errors.
struct {
uint pixels[];
} gfx;
#endif

// Get the relative tile coordinates given absolute pixel coordinates.
uvec2 get_tile_pixel(vec2 coord) {
    return uvec2(coord) & 7;
}

uint get_gfx_byte(uint index) {
    // The gfx data is an array of bytes packed into an array of 32-bit unsigned
    // integers. First, we must get the 32-bit integer that contains the byte we want.
    uint value = gfx.pixels[index >> 2];

    // The bit shift tells us which byte of the integer we want.
    uint bit_shift = (index & 3u) << 3;

    // Finally, return that byte
    return (value >> bit_shift) & 0xFF;
}

#if GFX_FORMAT==FORMAT_1BPP_8X8
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 3) + tile_pixel.y;
    return (get_gfx_byte(index) >> (tile_pixel.x ^ 7)) & 1u;
}
#elif GFX_FORMAT==FORMAT_2BPP_NES
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint result = 0;
    uint index = (tile_index << 4) + tile_pixel.y;
    tile_pixel.x ^= 7;
    result |= ((get_gfx_byte(index + 0x00u) >> tile_pixel.x) & 1u) << 0;
    result |= ((get_gfx_byte(index + 0x08u) >> tile_pixel.x) & 1u) << 1;
    return result;
}
#elif GFX_FORMAT==FORMAT_2BPP_GB
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint result = 0;
    uint index = (tile_index << 4) + (tile_pixel.y << 1);
    tile_pixel.x ^= 7;
    result |= ((get_gfx_byte(index + 0x00u) >> tile_pixel.x) & 1u) << 0;
    result |= ((get_gfx_byte(index + 0x01u) >> tile_pixel.x) & 1u) << 1;
    return result;
}
#elif GFX_FORMAT==FORMAT_2BPP_NGP
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint result = 0;
    uint index = (tile_index << 4) + (tile_pixel.y << 1);
    tile_pixel.x ^= 7;
    result |= get_gfx_byte(index + 0x00u) << 0;
    result |= get_gfx_byte(index + 0x01u) << 8;
    return ((result >> (tile_pixel.x << 1)) & 3u);
}
#elif GFX_FORMAT==FORMAT_2BPP_VB
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint result = 0;
    uint index = (tile_index << 4) + (tile_pixel.y << 1);
    tile_pixel.x ^= 7;
    result |= get_gfx_byte(index + 0x00u) << 0;
    result |= get_gfx_byte(index + 0x00u) << 8;
    return ((result >> (tile_pixel.x << 1)) & 3u);
}
#elif GFX_FORMAT==FORMAT_3BPP_8X8
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint result = 0;
    uint index = (tile_index * 0x18u) + tile_pixel.y;
    tile_pixel.x ^= 7;
    result |= ((get_gfx_byte(index + 0x00u) >> tile_pixel.x) & 1u) << 0;
    result |= ((get_gfx_byte(index + 0x08u) >> tile_pixel.x) & 1u) << 1;
    result |= ((get_gfx_byte(index + 0x10u) >> tile_pixel.x) & 1u) << 2;
    return result;
}
#elif GFX_FORMAT==FORMAT_3BPP_SNES
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint result = 0;
    uint offset = tile_index * uint(8 * 3);
    uint index1 = offset + (tile_pixel.y << 1);
    uint index2 = offset + tile_pixel.y;
    tile_pixel.x ^= 7;
    result |= ((get_gfx_byte(index1 + 0x00u) >> tile_pixel.x) & 1u) << 0;
    result |= ((get_gfx_byte(index1 + 0x01u) >> tile_pixel.x) & 1u) << 1;
    result |= ((get_gfx_byte(index2 + 0x10u) >> tile_pixel.x) & 1u) << 2;
    return result;
}
#elif GFX_FORMAT==FORMAT_4BPP_SNES
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint result = 0;
    uint index = (tile_index << 5) + (tile_pixel.y << 1);
    tile_pixel.x ^= 7;
    result |= ((get_gfx_byte(index + 0x00u) >> tile_pixel.x) & 1u) << 0;
    result |= ((get_gfx_byte(index + 0x01u) >> tile_pixel.x) & 1u) << 1;
    result |= ((get_gfx_byte(index + 0x10u) >> tile_pixel.x) & 1u) << 2;
    result |= ((get_gfx_byte(index + 0x11u) >> tile_pixel.x) & 1u) << 3;
    return result;
}
#elif GFX_FORMAT==FORMAT_4BPP_GBA
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 5) + (tile_pixel.y << 2);
    uint val = get_gfx_byte(index + (tile_pixel.x >> 1));

    return (val >> ((tile_pixel.x & 1u) << 2)) & 0x0F;
}
#elif GFX_FORMAT==FORMAT_4BPP_SMS
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint result = 0;
    uint index = (tile_index << 5) + (tile_pixel.y << 2);
    tile_pixel.x ^= 7;
    result |= ((get_gfx_byte(index + 0x00u) >> tile_pixel.x) & 1u) << 0;
    result |= ((get_gfx_byte(index + 0x01u) >> tile_pixel.x) & 1u) << 1;
    result |= ((get_gfx_byte(index + 0x02u) >> tile_pixel.x) & 1u) << 2;
    result |= ((get_gfx_byte(index + 0x03u) >> tile_pixel.x) & 1u) << 3;
    return result;
}
#elif GFX_FORMAT==FORMAT_4BPP_MSX2
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 5) + (tile_pixel.y << 2);
    uint val = get_gfx_byte(index + (tile_pixel.x >> 1));

    return (val >> (((tile_pixel.x & 1u) ^ 1u) << 2)) & 0x0Fu;
}
#elif GFX_FORMAT==FORMAT_4BPP_8X8
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint result = 0;
    uint index = (tile_index << 5) + tile_pixel.y;
    tile_pixel.x ^= 7;
    result |= ((get_gfx_byte(index + 0x00u) >> tile_pixel.x) & 1u) << 0;
    result |= ((get_gfx_byte(index + 0x08u) >> tile_pixel.x) & 1u) << 1;
    result |= ((get_gfx_byte(index + 0x10u) >> tile_pixel.x) & 1u) << 2;
    result |= ((get_gfx_byte(index + 0x18u) >> tile_pixel.x) & 1u) << 3;
    return result;
}
#elif GFX_FORMAT==FORMAT_8BPP_SNES
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    uint result = 0;
    uint index = (tile_index << 6) + (tile_pixel.y << 1);
    tile_pixel.x ^= 7;
    result |= ((get_gfx_byte(index + 0x00u) >> tile_pixel.x) & 1u) << 0;
    result |= ((get_gfx_byte(index + 0x01u) >> tile_pixel.x) & 1u) << 1;
    result |= ((get_gfx_byte(index + 0x10u) >> tile_pixel.x) & 1u) << 2;
    result |= ((get_gfx_byte(index + 0x11u) >> tile_pixel.x) & 1u) << 3;
    result |= ((get_gfx_byte(index + 0x20u) >> tile_pixel.x) & 1u) << 4;
    result |= ((get_gfx_byte(index + 0x21u) >> tile_pixel.x) & 1u) << 5;
    result |= ((get_gfx_byte(index + 0x30u) >> tile_pixel.x) & 1u) << 6;
    result |= ((get_gfx_byte(index + 0x31u) >> tile_pixel.x) & 1u) << 7;
    return result;
}
#elif GFX_FORMAT==FORMAT_8BPP_MODE7
uint get_color_index(uint tile_index, uvec2 tile_pixel) {
    return get_gfx_byte((tile_index << 6) + (tile_pixel.y * 8u) + tile_pixel.x);
}
#else
#error "GFX_FORMAT is either undefined or defined improperly"
#endif

#endif
