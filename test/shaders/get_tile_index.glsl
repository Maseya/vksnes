#ifndef GET_TILE_INDEX_GLSL
#define GET_TILE_INDEX_GLSL

#ifndef TILE_SIZE
#define TILE_SIZE 8
#endif

#ifndef TILE_WIDTH
#define TILE_WIDTH TILE_SIZE
#endif

#ifndef TILE_HEIGHT
#define TILE_HEIGHT TILE_SIZE
#endif

uint get_tile_index(vec2 pixel, uint stride) {
    uint x = uint(pixel.x / TILE_WIDTH);
    uint y = uint(pixel.y / TILE_HEIGHT);
    return (y * stride) + x;
}

#endif