#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform SizeBuffer {
    uvec2 size;
} frame;

layout(binding = 1) uniform PatternBuffer {
    vec4 colors[2];
    float angle;
    uint steps;
} pattern;

#define PI float(3.14159265358979323846)

void main() {
    // Get the pivot angle between the vertical bisector of the frame and the top-right
    // radius. Range is (0, PI/2) since x and y are both always positive and finite.
    float pivot = atan(frame.size.x, frame.size.y);

    // Get the angle between the gradient axis and the nearest radius.
    float axis_angle = mod(pattern.angle, PI) < PI / 2
        ? pivot - pattern.angle
        : pivot + pattern.angle;

    // The distance from the center of the frame to any corner.
    float radius = length(frame.size) / 2;

    // This measures the maximum distance a pixel inside the frame can be from the
    // center, measured along the rotated axis of our given angle.
    float dist = radius * abs(cos(axis_angle));

    // Gets the location of the frag coord relative to the center of the rectangle,
    // along the usual axes.
    vec2 real_point = gl_FragCoord.xy - (frame.size / 2);

    // The rotation matrix generated from our input angle.
    float cos_angle = cos(pattern.angle);
    float sin_angle = sin(pattern.angle);
    mat2 rotation = mat2(cos_angle, -sin_angle, sin_angle, cos_angle);

    // Rotate the real point to be measured along the gradient axes.
    vec2 rotated_point = rotation * real_point;

    // Scales the rotated point to be between -1 and 1.
    vec2 transfer_point = rotated_point / dist;

    // The mix value is used for applying the actual gradient.
    float mix_value = (transfer_point.y + 1) / 2;

    // Can be used to set discrete steps in the gradient.
    if (pattern.steps > 1) {
        mix_value -= mod(mix_value, 1.0f / pattern.steps);
        mix_value *= pattern.steps / (pattern.steps - 1);
    }

    // Special case to just draw the first color.
    else if (pattern.steps == 1) {
        mix_value = 0;
    }

    FragColor = mix(pattern.colors[0], pattern.colors[1], mix_value);
}