#include "vec.cuh"
#include "../test_runner.h"
#include <cassert>

__global__ void cross_3d_vec_kernel(const vec<3> *v1, const vec<3> *v2, vec<3> *v3) {
	*v3 = *v1 ^ *v2;
}

void cross_3d_vec_cu() {
	const float epsilon = 2e-6;
	vec<3> *v1, *v2, *v3;

	cudaMallocManaged(&v1, sizeof(vec<3>));
	cudaMallocManaged(&v2, sizeof(vec<3>));
	cudaMallocManaged(&v3, sizeof(vec<3>));

	*v1 = init_vec<3>();
	*v2 = init_vec<3>();

	cross_3d_vec_kernel<<<1, 1>>>(v1, v2, v3);
	cudaDeviceSynchronize();
	
	vec<3> check_vec;

	check_vec.x = v1->y * v2->z - v1->z * v2->y;
	check_vec.y = v1->z * v2->x - v1->x * v2->z;
	check_vec.z = v1->x * v2->y - v1->y * v2->x;

	for (int i = 0; i < 3; i++) {
		assert(check_vec.data[i] - epsilon <= v3->data[i] && check_vec.data[i] + epsilon >= v3->data[i]);
	}

	cudaFree(v1);
    cudaFree(v2);
    cudaFree(v3);
}

void cross_3d_vec_cpp() {
	const vec<3> v1 = init_vec<3>(), v2 = init_vec<3>();
	vec<3> v3 = v1 ^ v2;

	vec<3> check_vec;

	check_vec.x = v1.y * v2.z - v1.z * v2.y;
	check_vec.y = v1.z * v2.x - v1.x * v2.z;
	check_vec.z = v1.x * v2.y - v1.y * v2.x;

	assert(check_vec == v3);
}

int main() {
	cross_3d_vec_cpp();
	cross_3d_vec_cu();
    return 0;
}
