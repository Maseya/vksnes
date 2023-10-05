#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform SizeBuffer {
    uvec2 size;
} frame;

layout(std430, binding = 1) readonly buffer Selection{
uint size;
uint[] colors;
} selection;

layout(binding = 2) uniform PaletteBuffer {
// The view tile extent.
uvec2 size;
} view;

layout(location = 0) uniform HighlightColor {
uvec4 color;
} highlight;

vec4 get_highlight_color(uint index);
vec4 get_highlight_color(uvec2 tile);
vec4 get_highlight_color(uint x, uint y);

uvec2 get_tile() {
    // Get the discrete, integer pixel coordinate of this fragment.
    uvec2 pixel = uvec2(gl_FragCoord.x - 0.5, gl_FragCoord.y - 0.5);

    // Get the tile position of the pixel coordinate.
    uvec2 pixels_per_tile = frame.size / view.size;
    return pixel / pixels_per_tile;
}

uint get_index(uvec2 tile);

uint get_index(uint x, uint y) {
    return (y * view.size.x) + x;
}

void main() {
    // Get the palette color at this tile coordinate.
    FragColor = get_highlight_color(get_tile());
}

vec4 get_highlight_color(uint index) {
    if (index >= selection.size)
        discard;

    if (selection.colors[index] == 0)
        discard;

    return highlight.color;
}