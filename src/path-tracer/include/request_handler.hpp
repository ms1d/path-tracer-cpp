#pragma once

#include "structs.cuh"

void handle_request(int current_request, Pixel** buffers, bool &lock_state, int buffers_count);
