#version 450
#extension GL_KHR_vulkan_glsl : enable

#define VERSION_DEFINED

layout(location = 0) flat in uint TileIndex;
layout(location = 1) flat in vec2 FrameSize;
layout(location = 2) flat in uvec2 ViewSize;

layout(location = 0) out vec4 FragColor;

layout(binding = 3) readonly buffer Palette {
    uint[0x80] colors;
} palette;

layout(binding = 4) readonly buffer Gfx {
    uint[0x4000] pixels;
} gfx;

layout(binding = 5) readonly buffer ObjBuffer {
    uint[0x8000] tiles;
    uint size;
} obj;

void main() {
    FragColor = vec4(1);
}