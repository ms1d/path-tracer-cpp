#include "renderer.cuh"



#include <atomic>
#include <cstdint>
#include <random>



std::mt19937 rng(std::random_device{}());
std::uniform_real_distribution<float> dist(1.0, 2.0);



void start_render(
	uint16_t **pixels_to_skip, uint32_t pixels_to_skip_len,
    vec<3> *verts, uint32_t verts_len,
    uint32_t *tris, uint32_t tris_len,
    Materials mats,
    uint32_t *mat_indices, uint32_t mat_indices_len,
    vec<3> cam_pos, vec<3> cam_dir, float fov,
	Pixel **multi_buffer, std::atomic<int>& curr_gpu_write_count, uint64_t request_size, Pixel *cuda_buffers) {

	// orchestration algorithm: using request size, group into buffers and sequentially launch kernels
	curr_gpu_write_count = -1;
	std::cout << "gpu is done" << std::endl;
}
