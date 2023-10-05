// checker.frag
// Copyright (C) 2022 Nelson Garcia
//
// Fragment shader to output a checkerboard pattern.
//
// This source is public domain.

#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform PatternBuffer {
    // Holds the two possible colors for the checkerboard. The color at the first index
    // will go to the top-left corner of the output image.
    vec4 colors[2];

    // Pixel size of an individual checker cell.
    vec2 size;
} pattern;

void main() {
    // Get the discrete checkerboard coordinate containing the fragment.
    ivec2 tile = ivec2((gl_FragCoord.xy - 0.5) / pattern.size);

    // We determine which checkerboard color to used based on the parity of the tile
    // coordinates. If the first bits of the coordinates are the same, then we use the
    // first color (at index 0). Otherwise, we use the second color. We use the XOR
    // operator to determine bit equality.
    uint index = (tile.x ^ tile.y) & 1;

    // Finally, we assign the desired color.
    FragColor = pattern.colors[index];
}