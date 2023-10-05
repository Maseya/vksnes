#version 450
#extension GL_KHR_vulkan_glsl : enable

#ifdef TEX_COORDS
layout (location = 0) in float texCoord;
#endif

layout (location = 0) out vec4 FragColor;

#ifdef TEX_COORDS
layout(binding = 2) uniform sampler1D tex;
#endif

void main() {
#ifdef TEX_COORDS
	FragColor = texture(tex, texCoord);
#else
	FragColor = vec4(1);
#endif
}