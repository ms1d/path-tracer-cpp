#pragma once



#include <cstdint>
#include "vec.cuh"



#ifndef __host__
#define __host__
#endif
#ifndef __device__
#define __device__
#endif



// Should fit inside of 16 bytes to aid GPU performance
struct alignas(16) Pixel {
	float r,g,b;
	uint16_t x,y;
};

struct GpuQueue {
	Pixel* buffer;
	int* tail;
	int capacity;
};

struct Materials {
	vec<3>* colors;
	float *smoothness, *metallic;
	uint32_t length;
};
