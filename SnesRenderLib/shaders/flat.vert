#version 450
#extension GL_KHR_vulkan_glsl : enable

#ifdef TEX_COORDS
layout(location = 0) out vec2 texCoords;
#endif

void main() {
    // Equivalent to accessing array [(0, 0), (0, 2), (2, 0)] using gl_VertexIndex as
    // array index.
    ivec2 p = ivec2((gl_VertexIndex & 1) << 1, gl_VertexIndex & 2);

#ifdef TEX_COORDS
    texCoords = vec2(p);
#endif

    // Transform the coordinates to clip space.
    gl_Position = vec4((2 * p) - 1, 0, 1);
}
