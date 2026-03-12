#include "testkernel.h"
#include "vec3.h"
#include <cmath>
#include <curand_kernel.h>

__global__ void addNums(vec3* A, vec3* B, vec3* C, const int arrLength) {
	int id = threadIdx.x + blockDim.x * blockIdx.x;

	if (id >= arrLength) return;

	C[id] = A[id] + B[id];
}

__global__ void initArray(vec3* arr, const int arrLength, unsigned long seed) {
	int id = threadIdx.x + blockDim.x * blockIdx.x;
	if (id > arrLength) return;

	curandState state;
	curand_init(seed, id, 0, &state);

	arr[id].x = curand_uniform(&state);
	arr[id].y = curand_uniform(&state);
	arr[id].z = curand_uniform(&state);
}


// Basic function which adds numbers via a cuda kernel
testResult launchTest() {
	const int arrLength = 4096;
	const int threads = 256;

	vec3* A = nullptr;
	vec3* B = nullptr;
	vec3* C = nullptr;

	cudaMallocManaged(&A, arrLength*sizeof(vec3));
    cudaMallocManaged(&B, arrLength*sizeof(vec3));
    cudaMallocManaged(&C, arrLength*sizeof(vec3));

	int blocks = ceil((float)arrLength / threads);

	long seed = 102746194;
	initArray<<<blocks, threads>>>(A, arrLength, seed);
	initArray<<<blocks, threads>>>(B, arrLength, seed << 27);
	addNums<<<blocks, threads>>>(A, B, C, arrLength);

    cudaDeviceSynchronize();

	testResult res;
	res.A = A;
	res.B = B;
	res.C = C;
	res.arrLength = arrLength;
	return res;
}
