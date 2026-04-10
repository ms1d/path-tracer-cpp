#include "buffer_setup.cuh"

void alloc(Pixel *&buffers, int buffers_count, int buffer_length) { cudaMalloc(&buffers, buffers_count * buffer_length * sizeof(Pixel)); }

void dealloc(Pixel *&buffers) { cudaFree(&buffers); }
