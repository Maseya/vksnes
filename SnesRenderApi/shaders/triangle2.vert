#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color3;

layout(location = 0) out vec3 frag_color;

layout(binding = 0) uniform AngleBuffer {
    float angle;
}ab;

void main() {
    mat2 rotator = mat2(cos(ab.angle), sin(ab.angle), -sin(ab.angle), cos(ab.angle));
    gl_Position = vec4(rotator * in_position, 0, 1);
    frag_color = in_color3;
}
