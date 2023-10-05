#version 450
#extension GL_KHR_vulkan_glsl : enable

// RGB channels for a SNES color.
#define R_CHANNEL 0
#define G_CHANNEL 1
#define B_CHANNEL 2

layout(location = 0) flat in uint TileIndex;

layout(location = 0) out vec4 FragColor;

layout(binding = 2) readonly buffer PaletteBuffer {
uint[0x8000] colors;
uint size;
} palette;

uint get_palette_color(uint index);
vec3 get_rgb_color(uint palette_color);
uint get_palette_color_component(uint palette_color, int channel);
float get_color_intensity(uint component);

void main() {
    if (TileIndex >= palette.size) discard;

    uint palette_color = get_palette_color(TileIndex);
    FragColor = vec4(get_rgb_color(palette_color), 1);
}

uint get_palette_color(uint index) {
    // Get the actual index into the palette.colors array. We are packing the palette's
    // 16-bit colors into a 32-bit array, so two colors actually exist per array
    // element.
    uint array_index = index >> 1;

    // Let's us get either the high or low word of that array element.
    uint bit_shift = (index & 1) << 4;

    // Don't forget to reduce the result to 16 bits.
    return (palette.colors[array_index] >> bit_shift) & 0xffff;
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