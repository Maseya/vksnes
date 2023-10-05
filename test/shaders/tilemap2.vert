#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) flat out uint TileIndex;

#ifndef PALETTE_VERTEX
layout(location = 1) flat out vec2 FrameSize;
layout(location = 2) flat out uvec2 ViewSize;
#endif

layout(binding = 0) uniform SizeBuffer {
    uvec2 size;
} frame;

layout(binding = 1) uniform BoundaryBuffer {
    vec4 bounds;
} boundary;

layout(binding = 2) uniform ViewBuffer {
    uvec2 size;
} view;

void main() {
    TileIndex = gl_InstanceIndex;
#ifndef PALETTE_VERTEX
    FrameSize = boundary.bounds.zw;
    ViewSize = view.size;
#endif

#ifdef MAP16_TILEMAP
#endif

    vec2 offset = vec2(gl_VertexIndex & 1, gl_VertexIndex >> 1);
    gl_Position = vec4((2 * offset) - 1, 0, 1);
}