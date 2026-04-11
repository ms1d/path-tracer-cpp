#include "buffer_setup.cuh"

void alloc(Pixel *&buffers, uint max_requests, uint buffer_count, uint buffer_length) {
	cudaMalloc(&buffers, max_requests * buffer_count * buffer_length * sizeof(Pixel));
}

void dealloc(Pixel *&buffers) { cudaFree(&buffers); }
