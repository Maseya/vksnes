#ifndef GET_OBJ_PROPERTIES_GLSL
#define GET_OBJ_PROPERTIES_GLSL

#ifndef VERSION_DEFINED
#extension GL_EXT_gpu_shader4 : enable

// Dummy struct to remove Intellisense errors.
struct {
uint tiles[];
} obj;
#endif

uint get_gfx_tile_index(uint obj_tile) {
	return obj_tile & 0x3FFu;
}

uint get_palette_offset(uint obj_tile) {
	return (obj_tile >> 6) & 0x70u;
}

uint get_flip_mask(uint obj_tile) {
	return (obj_tile >> 14) & 3;
}

uint get_x_flip_mask(uint obj_tile) {
	return ((obj_tile >> 14) & 1) * 7;
}

uint get_y_flip_mask(uint obj_tile) {
	return ((obj_tile >> 15) & 1) * 7;
}

#endif