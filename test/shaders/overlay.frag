#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 pos;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D tex;

void main() {
	FragColor = texture(tex, pos);
}