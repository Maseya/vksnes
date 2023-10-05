#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) flat out uint TileIndex;

// The vertex shader for the palette pipeline does not use these.
#ifndef PALETTE_VERTEX
#ifdef GFX_VERTEX
layout(location = 1) flat out vec2 FrameSize;
layout(location = 2) flat out uvec2 ViewSize;
#else
layout(location = 1) out vec2 Offset;
#endif
#endif

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
#ifdef GFX_VERTEX
    FrameSize = frame.size;
    ViewSize = view.size;
#endif

#ifdef MAP16_TILEMAP
    // Setting the tile index for a Map16 layout is a little more complicated compared
    // to the normal method. The first tile of every two rows has the same index in map8
    // or map16 mode. So we calculate the relative index to the second row and base our
    // calculations from there.
    uint relative_index = gl_InstanceIndex % (view.size.x << 1);

    // This essentially lets us double the relative index. For tiles on the first
    // relative row, this is all we need to do.
    TileIndex += relative_index;
    if (relative_index >= view.size.x) {
        // However, for tiles on the second row, we need to add 1 because these tiles
        // will all have odd indices. We also subtract two rows as a way of doing
        // modulo, which will be required since the relative index will take us over.
        TileIndex += 1 - (view.size.x << 1);
    }
#endif

    // We are emitting vertices for the corners of the tile. First, we are going to get
    // the pixel coordinates of the to-be-emitted vertex.
    uvec2 offset = uvec2(gl_VertexIndex & 1, gl_VertexIndex >> 1);
#ifndef PALETTE_VERTEX
#ifndef GFX_VERTEX
    Offset = vec2(offset);
#endif
#endif

    uvec2 tile = uvec2(gl_InstanceIndex % view.size.x, gl_InstanceIndex / view.size.x);
    vec2 pixels_per_tile = vec2(frame.size) / view.size;
    vec2 pixel = ceil(pixels_per_tile * (tile + offset)) + 0.5;

    // Once we have the pixel coordinate, we transform it to clip coordinates and emit.
    gl_Position = vec4((2 * pixel / frame.size) - 1, 0, 1);
}