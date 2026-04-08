#include "renderer.cuh"



#include <atomic>
#include <cstdint>
#include <random>



std::mt19937 rng(std::random_device{}());
std::uniform_real_distribution<float> dist(1.0, 2.0);



void start_render(
    vec<3> *verts, uint32_t verts_len,
    uint32_t *tris, uint32_t tris_len,
    Materials mats,
    uint32_t *mat_indices, uint32_t mat_indices_len,
    vec<3> cam_pos, vec<3> cam_dir, float fov,
	uint8_t tilesize,
	Pixel** buffer, std::atomic<int>& curr_buffer, int buffers_count) {

	while (curr_buffer < buffers_count) {
		int curr_buffer_index = curr_buffer % buffers_count;

		// simulate work
		sleep(dist(rng));

		curr_buffer++;
		std::cout << "Moved gpu buffer up to " << curr_buffer << std::endl;	
	}

	curr_buffer = -1;
	std::cout << "gpu is done" << std::endl;
}
