#version 450
#extension GL_KHR_vulkan_glsl : enable
#define VERSION_DEFINED

layout (location = 0) in uvec2 coord;

layout (location = 1) in uint tile;

layout (location = 0) flat out uvec2 Pixel;

layout (location = 1) flat out uint ChrTile;

layout(binding = 0) uniform Frame {
    uvec2 size;
} frame;

void main() {
    ivec2 tile_corner = ivec2(gl_VertexIndex & 1, (gl_VertexIndex & 2) >> 1);
    vec2 p = ((tile_corner * 8.0f) + coord + 0.5f) / frame.size;

    Pixel = coord;
    ChrTile = tile;
    gl_Position = vec4((2 * p) - 1, 0 , 1);
}