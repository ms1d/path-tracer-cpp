#pragma once

#include "structs.cuh"



void alloc(Pixel *&buffers, int buffers_count, int buffer_length);

void dealloc(Pixel *&buffers);
