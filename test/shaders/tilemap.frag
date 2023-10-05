#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform SizeBuffer {
    uvec2 size;
} frame;

layout(binding = 1) readonly buffer ObjBuffer {
uint[0x40000] tiles;
uint size;
} obj;

layout(binding = 2) readonly buffer Gfx {
uint[0x4000] pixels;
uint[0x80] colors;
} gfx;

layout(binding = 3) uniform ViewBuffer {
uvec2 size;
int format;
int map16;
} view;

// We subtract by 0.5 because fragment coordinates are described at the center of the
// pixel rather than the corner.
#define CENTER_PIXEL_TO_CORNER(coord) ((coord)-0.5f)

// RGB channels for a SNES color.
#define R_CHANNEL 0
#define G_CHANNEL 1
#define B_CHANNEL 2

// Supported graphics bit formats.
#define FORMAT_1BPP_8X8     0x11
#define FORMAT_2BPP_NES     0x22
#define FORMAT_2BPP_GB      0x32
#define FORMAT_2BPP_NGP     0x42
#define FORMAT_2BPP_VB      0x52
#define FORMAT_3BPP_SNES    0x03
#define FORMAT_3BPP_8X8     0x13
#define FORMAT_4BPP_SNES    0x04
#define FORMAT_4BPP_GBA     0x14
#define FORMAT_4BPP_SMS     0x24
#define FORMAT_4BPP_MSX2    0x34
#define FORMAT_4BPP_8X8     0x44
#define FORMAT_8BPP_SNES    0x08
#define FORMAT_8BPP_MODE7   0x78

#define BPP_SIZE(format) ((format) & 0x0F)

uvec2 get_tile(vec2 coord);
uint get_tile_index(uvec2 tile);

uint get_obj_tile(uint index);

uint get_gfx_byte(uint index);

uvec2 get_tile_pixel(vec2 coord);
uint get_pixel_index(uvec2 tile_pixel);

uint get_color_index(int format, uint tile_index, uvec2 tile_pixel);
uint get_color_index_1bpp_8x8(uint tile_index, uvec2 tile_pixel);
uint get_color_index_2bpp_nes(uint tile_index, uvec2 tile_pixel);
uint get_color_index_2bpp_gb(uint tile_index, uvec2 tile_pixel);
uint get_color_index_2bpp_ngp(uint tile_index, uvec2 tile_pixel);
uint get_color_index_2bpp_vb(uint tile_index, uvec2 tile_pixel);
uint get_color_index_3bpp_snes(uint tile_index, uvec2 tile_pixel);
uint get_color_index_3bpp_8x8(uint tile_index, uvec2 tile_pixel);
uint get_color_index_4bpp_snes(uint tile_index, uvec2 tile_pixel);
uint get_color_index_4bpp_gba(uint tile_index, uvec2 tile_pixel);
uint get_color_index_4bpp_sms(uint tile_index, uvec2 tile_pixel);
uint get_color_index_4bpp_msx2(uint tile_index, uvec2 tile_pixel);
uint get_color_index_4bpp_8x8(uint tile_index, uvec2 tile_pixel);
uint get_color_index_8bpp_snes(uint tile_index, uvec2 tile_pixel);
uint get_color_index_8bpp_mode7(uint tile_index, uvec2 tile_pixel);

uint get_palette_color(uint index);
vec3 get_rgb_color(uint palette_color);
uint get_palette_color_component(uint palette_color, int channel);
float get_color_intensity(uint component);

void main() {
    uvec2 tile = get_tile(gl_FragCoord.xy);
    uint tile_index = get_tile_index(tile);
    int format = view.format;
    if (tile_index >= obj.size / 2)
        discard;

    uint obj_tile = get_obj_tile(tile_index);
    uint gfx_tile_index = obj_tile & 0x3FF;
    uint palette_offset = ((obj_tile >> 10) & 7) << 4;
    uint flip_mask = (obj_tile >> 14) & 3;
    uvec2 tile_pixel = get_tile_pixel(gl_FragCoord.xy);
    tile_pixel.x ^= (flip_mask & 1) * 7;
    tile_pixel.y ^= ((flip_mask & 2) >> 1) * 7;
    uint color_index = get_color_index(format, gfx_tile_index, tile_pixel);

    uint palette_color = get_palette_color(color_index + palette_offset);
    FragColor = vec4(get_rgb_color(palette_color), color_index > 0 ? 1 : 0);
}

uvec2 get_tile(vec2 coord) {
    // Get the discrete, integer pixel coordinate.
    uvec2 pixel = uvec2(CENTER_PIXEL_TO_CORNER(coord));

    vec2 pixels_per_tile = vec2(frame.size) / view.size;
    return uvec2(pixel / pixels_per_tile);
}

uint get_tile_index(uvec2 tile) {
    // Tiles are ordered like an English book: top to bottom then left to right.
    return (tile.y * view.size.x) + tile.x;
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

uint get_gfx_byte(uint index) {
    // Get the actual index into the gfx.pixels array. The gfx data is an array of
    // bytes packed into an array of 32-bit unsigned integers.
    uint array_index = index >> 2;

    // The bit shift tells us which byte of the integer we want.
    uint bit_shift = (index & 3) << 3;

    // Finally, return the desired value as an 8-bit integer.
    return (gfx.pixels[array_index] >> bit_shift) & 0xFF;
}

uvec2 get_tile_pixel(vec2 coord) {
    // A tile in this context is an 8x8 GFX tile. We first determine how many screen
    // pixels are in each tile.
    vec2 pixels_per_tile = vec2(frame.size) / (view.size * (view.map16 + 1));

    // We then get the screen pixel coordinate relative to the tile that we are in.
    vec2 rem = mod(CENTER_PIXEL_TO_CORNER(coord), pixels_per_tile);

    // Finally, we scale it to an 8x8 tile pixel.
    return uvec2((rem / pixels_per_tile) * 8);
}

uint get_pixel_index(uvec2 pixel) {
    return (pixel.y * 8) + pixel.x;
}

uint get_color_index(int format, uint tile_index, uvec2 tile_pixel) {
switch (format) {
    case FORMAT_1BPP_8X8:
        return get_color_index_1bpp_8x8(tile_index, tile_pixel);
    case FORMAT_2BPP_NES:
        return get_color_index_2bpp_nes(tile_index, tile_pixel);
    case FORMAT_2BPP_GB:
        return get_color_index_2bpp_gb(tile_index, tile_pixel);
    case FORMAT_2BPP_NGP:
        return get_color_index_2bpp_ngp(tile_index, tile_pixel);
    case FORMAT_2BPP_VB:
        return get_color_index_2bpp_vb(tile_index, tile_pixel);
    case FORMAT_3BPP_SNES:
        return get_color_index_3bpp_snes(tile_index, tile_pixel);
    case FORMAT_3BPP_8X8:
        return get_color_index_3bpp_8x8(tile_index, tile_pixel);
    case FORMAT_4BPP_SNES:
        return get_color_index_4bpp_snes(tile_index, tile_pixel);
    case FORMAT_4BPP_GBA:
        return get_color_index_4bpp_gba(tile_index, tile_pixel);
    case FORMAT_4BPP_SMS:
        return get_color_index_4bpp_sms(tile_index, tile_pixel);
    case FORMAT_4BPP_MSX2:
        return get_color_index_4bpp_msx2(tile_index, tile_pixel);
    case FORMAT_4BPP_8X8:
        return get_color_index_4bpp_8x8(tile_index, tile_pixel);
    case FORMAT_8BPP_SNES:
        return get_color_index_8bpp_snes(tile_index, tile_pixel);
    case FORMAT_8BPP_MODE7:
        return get_color_index_8bpp_mode7(tile_index, tile_pixel);
    default:
        return 0;
    }
}

uint get_color_index_1bpp_8x8(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 3) + tile_pixel.y;
    uint val1 = get_gfx_byte(index);
    tile_pixel.x ^= 7;

    return (val1 >> tile_pixel.x) & 1;
}

uint get_color_index_2bpp_nes(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 4) + tile_pixel.y;
    uint val1 = get_gfx_byte(index + 0x00);
    uint val2 = get_gfx_byte(index + 0x08);
    tile_pixel.x ^= 7;

    return
        (((val1 >> tile_pixel.x) & 1) << 0) |
        (((val2 >> tile_pixel.x) & 1) << 1);
}

uint get_color_index_2bpp_gb(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 4) + (tile_pixel.y << 1);
    uint val1 = get_gfx_byte(index + 0x00);
    uint val2 = get_gfx_byte(index + 0x01);
    tile_pixel.x ^= 7;

    return
        (((val1 >> tile_pixel.x) & 1) << 0) |
        (((val2 >> tile_pixel.x) & 1) << 1);
}

uint get_color_index_2bpp_ngp(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 4) + (tile_pixel.y << 1);
    uint val1 = get_gfx_byte(index + 0x00);
    uint val2 = get_gfx_byte(index + 0x01);
    uint val = val1 | (val2 << 8);
    tile_pixel.x ^= 7;

    return ((val >> (tile_pixel.x << 1)) & 3);
}

uint get_color_index_2bpp_vb(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 4) + (tile_pixel.y << 1);
    uint val1 = get_gfx_byte(index + 0x00);
    uint val2 = get_gfx_byte(index + 0x01);
    uint val = val1 | (val2 << 8);

    return ((val >> (tile_pixel.x << 1)) & 3);
}

uint get_color_index_3bpp_8x8(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index * 0x18) + tile_pixel.y;
    uint val1 = get_gfx_byte(index + 0x00);
    uint val2 = get_gfx_byte(index + 0x08);
    uint val3 = get_gfx_byte(index + 0x10);
    tile_pixel.x ^= 7;

    return
        (((val1 >> tile_pixel.x) & 1) << 0) |
        (((val2 >> tile_pixel.x) & 1) << 1) |
        (((val3 >> tile_pixel.x) & 1) << 2);
}

uint get_color_index_3bpp_snes(uint tile_index, uvec2 tile_pixel) {
    uint offset = tile_index * (8 * 3);
    uint index1 = offset + (tile_pixel.y << 1);
    uint index2 = offset + tile_pixel.y;
    uint val1 = get_gfx_byte(index1 + 0x00);
    uint val2 = get_gfx_byte(index1 + 0x01);
    uint val3 = get_gfx_byte(index2 + 0x10);
    tile_pixel.x ^= 7;

    return
        (((val1 >> tile_pixel.x) & 1) << 0) |
        (((val2 >> tile_pixel.x) & 1) << 1) |
        (((val3 >> tile_pixel.x) & 1) << 2);
}

uint get_color_index_4bpp_snes(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 5) + (tile_pixel.y << 1);
    uint val1 = get_gfx_byte(index + 0x00);
    uint val2 = get_gfx_byte(index + 0x01);
    uint val3 = get_gfx_byte(index + 0x10);
    uint val4 = get_gfx_byte(index + 0x11);
    tile_pixel.x ^= 7;

    return
        (((val1 >> tile_pixel.x) & 1) << 0) |
        (((val2 >> tile_pixel.x) & 1) << 1) |
        (((val3 >> tile_pixel.x) & 1) << 2) |
        (((val4 >> tile_pixel.x) & 1) << 3);
}

uint get_color_index_4bpp_gba(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 5) + (tile_pixel.y << 2);
    uint val = get_gfx_byte(index + (tile_pixel.x >> 1));

    return (val >> ((tile_pixel.x & 1) << 2)) & 0x0F;
}
uint get_color_index_4bpp_sms(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 5) + (tile_pixel.y << 2);
    uint val1 = get_gfx_byte(index + 0x00);
    uint val2 = get_gfx_byte(index + 0x01);
    uint val3 = get_gfx_byte(index + 0x02);
    uint val4 = get_gfx_byte(index + 0x03);
    tile_pixel.x ^= 7;

    return
        (((val1 >> tile_pixel.x) & 1) << 0) |
        (((val2 >> tile_pixel.x) & 1) << 1) |
        (((val3 >> tile_pixel.x) & 1) << 2) |
        (((val4 >> tile_pixel.x) & 1) << 3);
}

uint get_color_index_4bpp_msx2(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 5) + (tile_pixel.y << 2);
    uint val = get_gfx_byte(index + (tile_pixel.x >> 1));

    return (val >> (((tile_pixel.x & 1) ^ 1) << 2)) & 0x0F;
}

uint get_color_index_4bpp_8x8(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 5) + tile_pixel.y;
    uint val1 = get_gfx_byte(index + 0x00);
    uint val2 = get_gfx_byte(index + 0x08);
    uint val3 = get_gfx_byte(index + 0x10);
    uint val4 = get_gfx_byte(index + 0x18);
    tile_pixel.x ^= 7;

    return
        (((val1 >> tile_pixel.x) & 1) << 0) |
        (((val2 >> tile_pixel.x) & 1) << 1) |
        (((val3 >> tile_pixel.x) & 1) << 2) |
        (((val4 >> tile_pixel.x) & 1) << 3);
}

uint get_color_index_8bpp_snes(uint tile_index, uvec2 tile_pixel) {
    uint index = (tile_index << 6) + (tile_pixel.y << 1);
    uint val1 = get_gfx_byte(index + 0x00);
    uint val2 = get_gfx_byte(index + 0x01);
    uint val3 = get_gfx_byte(index + 0x10);
    uint val4 = get_gfx_byte(index + 0x11);
    uint val5 = get_gfx_byte(index + 0x20);
    uint val6 = get_gfx_byte(index + 0x21);
    uint val7 = get_gfx_byte(index + 0x30);
    uint val8 = get_gfx_byte(index + 0x31);
    tile_pixel.x ^= 7;

    return
        (((val1 >> tile_pixel.x) & 1) << 0) |
        (((val2 >> tile_pixel.x) & 1) << 1) |
        (((val3 >> tile_pixel.x) & 1) << 2) |
        (((val4 >> tile_pixel.x) & 1) << 3) |
        (((val5 >> tile_pixel.x) & 1) << 4) |
        (((val6 >> tile_pixel.x) & 1) << 5) |
        (((val7 >> tile_pixel.x) & 1) << 6) |
        (((val8 >> tile_pixel.x) & 1) << 7);
}

uint get_color_index_8bpp_mode7(uint tile_index, uvec2 tile_pixel) {
    return get_gfx_byte((tile_index << 6) + get_pixel_index(tile_pixel));
}

uint get_palette_color(uint index) {
    // Get the actual index into the palette.colors array. We are packing the palette's
    // 16-bit colors into a 32-bit array, so two colors actually exist per array
    // element.
    uint array_index = index >> 1;

    // Let's us get either the high or low word of that array element.
    uint bit_shift = (index & 1) << 4;

    // Don't forget to reduce the result to 16 bits.
    return (gfx.colors[array_index] >> bit_shift) & 0xffff;
}

vec3 get_rgb_color(uint palette_color) {
    return vec3(
        get_color_intensity(get_palette_color_component(palette_color, R_CHANNEL)),
        get_color_intensity(get_palette_color_component(palette_color, G_CHANNEL)),
        get_color_intensity(get_palette_color_component(palette_color, B_CHANNEL)));
}

uint get_palette_color_component(uint palette_color, int channel) {
    // The SNES color is stored in sequential 5 bits.
    return (palette_color >> (5 * channel)) & 0x1fu;
}

float get_color_intensity(uint component) {
    // Maximum value will be 248/255. Will never return an intensity of 1. If you want
    // to scale between 0 and 1, return float(component) / 31.0f.
    return float(component << 3) / 255.0f;
}
