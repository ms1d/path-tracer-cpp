#pragma once



#ifndef __host__
#define __host__
#endif
#ifndef __device__
#define __device__
#endif



#include "structs.cuh"
#include "vec.cuh"
#include <atomic>



void start_render(
	vec<3> *verts, uint32_t verts_len,
	uint32_t *tris, uint32_t tris_len,
	Materials mats,
	uint32_t *mat_indices, uint32_t mat_indices_len,
	vec<3> cam_pos, vec<3> cam_dir, float fov,
	uint8_t tilesize,
	Pixel **buffer, std::atomic<int> &curr_buffer, int buffers_count
);
