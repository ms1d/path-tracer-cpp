#pragma once



#ifndef __host__
#define __host__
#endif
#ifndef __device__
#define __device__
#endif



#include <cstdint>
#include "vec.cuh"



// Should fit inside of 16 bytes to aid GPU performance
struct alignas(16) Pixel {
	float r,g,b;
	uint16_t x,y;
};

struct Materials {
	vec<3> *rgb;
	float *smoothness, *metallic, *emission;
	uint8_t length;
};
