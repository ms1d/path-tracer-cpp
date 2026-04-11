#pragma once

#include "structs.cuh"



void alloc(Pixel *&buffers, uint max_requests, uint buffer_count, uint buffer_length);

void dealloc(Pixel *&buffers);
