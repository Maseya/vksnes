#version 450
#extension GL_KHR_vulkan_glsl : enable

#ifdef TEXTURE
layout(location = 0) out vec2 texCoords;
#endif

void main() {
    // Creates one of {0, 0}, {2, 0}, {0, 2}, or {2, 2} based on vertex index.
    ivec2 p = ivec2((gl_VertexIndex & 1) << 1, gl_VertexIndex & 2);

#ifdef TEXTURE
    texCoords = vec2(p);
#endif

    // The final position needs to be in the range [-1, 1]x[-1, 1].
    gl_Position = vec4((2 * p) - 1, 0, 1);
}
