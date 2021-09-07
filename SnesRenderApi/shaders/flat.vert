#version 450
#extension GL_KHR_vulkan_glsl : enable
layout(location = 0) out vec2 Size;

layout(binding = 0) uniform SizeBuffer {
    vec2 size;
}sb;

void main() {
    Size = sb.size;
    vec2 TexCoord = vec2(gl_VertexIndex & 1, gl_VertexIndex >> 1);
    gl_Position = vec4(2 * TexCoord - 1, 0, 1);
}
