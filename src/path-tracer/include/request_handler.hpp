#pragma once

#include "structs.cuh"

void handle_request(int current_request, Pixel *buffer, Pixel *cuda_buffers, bool &lock_state);
