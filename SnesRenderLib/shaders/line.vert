#version 450
#extension GL_KHR_vulkan_glsl : enable
#define VERSION_DEFINED

layout (location = 0) in vec2 p1;
layout (location = 1) in vec2 p2;

#ifdef TEX_COORDS
layout (location = 2) in float texCoord1;
layout (location = 3) in float texCoord2;

layout (location = 0) out float texCoord;
#endif

layout(binding = 0) uniform SizeBuffer {
    uvec2 size;
} frame;

layout(binding = 1) uniform LineBuffer {
    float width;
} line;

void main() {
    vec2 p = vec2(gl_VertexIndex & 1, ((gl_VertexIndex & 2) >> 1) - 0.5f);

    mat2 transformation;
    transformation[0] = p2 - p1;
    transformation[1] = line.width *
        normalize(vec2(-transformation[0].y, transformation[0].x));

#ifdef TEX_COORDS
    texCoord = mix(texCoord1, texCoord2, p.x);
#endif

    vec2 point = p1 + transformation * p;
    gl_Position = vec4((2 * point / vec2(frame.size)) - 1, 0, 1);
}
