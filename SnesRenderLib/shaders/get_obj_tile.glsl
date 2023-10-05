#ifndef GET_OBJ_TILE_GLSL
#define GET_OBJ_TILE_GLSL

#ifndef VERSION_DEFINED
#extension GL_EXT_gpu_shader4 : enable

// Dummy struct to remove Intellisense errors.
struct {
    uint[] tiles;
} obj;
#endif

uint get_obj_tile(uint index) {
    // Get the actual index into the palette.colors array. We are packing the palette's
    // 16-bit colors into a 32-bit array, so two colors actually exist per array
    // element.
    uint array_index = index >> 1;

    // Let's us get either the high or low word of that array element.
    uint bit_shift = (index & 1) << 4;

    // Don't forget to reduce the result to 16 bits.
    return (obj.tiles[array_index] >> bit_shift) & 0xffff;
}

#endif