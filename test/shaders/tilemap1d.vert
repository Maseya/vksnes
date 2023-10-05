#version 450
#extension GL_KHR_vulkan_glsl : enable

// Define a square tile using counter-clockwise oriented triangles.
uvec2 vertices[4] = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
int indexes[6] = {0, 1, 2, 2, 1, 3};

layout(location = 0) flat out uint TileIndex;
layout(location = 1) flat out uvec2 FrameSize;
layout(location = 2) flat out uvec2 ViewSize;

// Specifies the actual full size of the image/frame.
layout(binding = 0) uniform SizeBuffer {
    uvec2 size;
} frame;

// Specifies the dimensions of the view in tile units.
layout(binding = 1) uniform ViewBuffer {
uvec2 size;
} view;

void main() {
    TileIndex = gl_InstanceIndex;
    FrameSize = frame.size;
    ViewSize = view.size;

    // We are emitting vertices for the corners of the tile. First, we are going to get
    // the pixel coordinates of the to-be-emitted vertex.
    uvec2 offset = vertices[indexes[gl_VertexIndex]];
    uvec2 tile = uvec2(TileIndex % view.size.x, TileIndex / view.size.x);
    vec2 pixels_per_tile = vec2(frame.size) / view.size;
    vec2 pixel = floor(pixels_per_tile * (tile + offset)) + 0.5;

    // Once we have the pixel coordinate, we transform it to clip coordinates and emit.
    gl_Position = vec4((2 * pixel / frame.size) - 1, 0, 1);
}