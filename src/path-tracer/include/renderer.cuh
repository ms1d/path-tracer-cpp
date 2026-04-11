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
	uint16_t **pixels_to_skip, uint32_t pixels_to_skip_len,
	vec<3> *verts, uint32_t verts_len,
	uint32_t *tris, uint32_t tris_len,
	Materials mats,
	uint32_t *mat_indices, uint32_t mat_indices_len,
	vec<3> cam_pos, vec<3> cam_dir, float fov,
	Pixel **multi_buffer, std::atomic<int> &curr_buffer, uint64_t request_size, Pixel *cuda_buffers
);
