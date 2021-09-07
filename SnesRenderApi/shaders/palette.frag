#version 450

// The full extent of the output image.
layout(location = 0) in vec2 Size;

// Result color for this fragment.
layout(location = 0) out vec4 FragColor;

// Texture buffer of our palette data.
layout(std140, binding = 1) readonly buffer Palette{
uint[] colors;
} palette;

layout(binding = 2) uniform PaletteBuffer {
// Start index where we begin reading palette data.
int palette_start;

// The view tile extent.
ivec2 view;
} pb;

// Convert a 15-bit bgr color to an ARGB vector.
vec4 bgr15(uint color15);

// Get the palette color given its relative index into the palette array.
vec4 get_palette_color(int index);

// Get the palette color given its tile position.
vec4 get_palette_color(ivec2 tile);
vec4 get_palette_color(int x, int y);

void main()
{
    // Get the discrete, integer pixel coordinate of this fragment.
    ivec2 pixel = ivec2(gl_FragCoord.x - 0.5, (Size.y - gl_FragCoord.y) - 0.5);

    // Get the tile position of the pixel coordinate.
    vec2 pixels_per_tile = Size / pb.view;
    ivec2 tile = ivec2(pixel / pixels_per_tile);

    // Get the palette color at this tile coordinate.
    FragColor = get_palette_color(tile);
}

vec4 get_palette_color(int index)
{
    index += pb.palette_start;
    if (uint(index) >= palette.colors.length())
        return vec4(0);

    return bgr15(palette.colors[index >> 1] | (palette.colors[(index >> 1) + 1] << 8));
}

vec4 get_palette_color(ivec2 tile)
{
    return get_palette_color(tile.x, tile.y);
}

vec4 get_palette_color(int x, int y)
{
    return get_palette_color((y * pb.view.x) + x);
}

vec4 bgr15(uint color15)
{
    float r = float((color15 >> (5 * 0)) & 0x1fu) / 31.0;
    float g = float((color15 >> (5 * 1)) & 0x1fu) / 31.0;
    float b = float((color15 >> (5 * 2)) & 0x1fu) / 31.0;
    return vec4(r, g, b, 1.0);
}
