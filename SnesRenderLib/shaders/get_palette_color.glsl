#ifndef GET_PALETTE_COLOR_GLSL
#define GET_PALETTE_COLOR_GLSL

#ifndef VERSION_DEFINED
#extension GL_EXT_gpu_shader4 : enable

// Dummy struct to remove Intellisense errors.
struct {
uint colors[];
}palette;
#endif

// RGB channels for a SNES color.
#define R_CHANNEL 0
#define G_CHANNEL 1
#define B_CHANNEL 2

uint get_palette_color(uint index) {
    // We are packing the palette's 16-bit colors into a 32-bit array. So first, we need
    // the actual 32-bit integer that contains the color we want.
    uint value = palette.colors[index >> 1];

    // Now we need to determine which bit offset the color starts at.
    uint bit_shift = (index & 1u) << 4;

    // Finally, we return the desired color.
    return (value >> bit_shift) & 0xffff;
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

vec3 get_rgb_color(uint palette_color) {
    return vec3(
        get_color_intensity(get_palette_color_component(palette_color, R_CHANNEL)),
        get_color_intensity(get_palette_color_component(palette_color, G_CHANNEL)),
        get_color_intensity(get_palette_color_component(palette_color, B_CHANNEL)));
}

#endif