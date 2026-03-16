#include "vec.cuh"
#include "../test_runner.h"
#include <cassert>

template<size_t dim>
__global__ void scale_vec_kernel(const vec<dim>* v1, vec<dim>* v2, vec<dim>* res, float* scale) {
	*v2 = *v1 * *scale;
	*res = *scale * *v1;
}

template<size_t dim>
void scale_vec_cu() {
	vec<dim> *v1, *v2, *res;
	float *scale;

	cudaMallocManaged(&v1, sizeof(vec<dim>));
	cudaMallocManaged(&v2, sizeof(vec<dim>));
	cudaMallocManaged(&res, sizeof(vec<dim>));
	cudaMallocManaged(&scale, sizeof(float));

	*v1 = init_vec<dim>();
    *scale = dist(rng);

	scale_vec_kernel<<<1, 1>>>(v1, v2, res, scale);
	cudaDeviceSynchronize();

	assert(*v2 == *res);

	vec<dim> check_vec;
	for (size_t i = 0; i < dim; i++) {
		check_vec.data[i] = v1->data[i] * *scale;
	}

	assert(*v2 == check_vec);

	cudaFree(v1);
	cudaFree(v2);
	cudaFree(res);
	cudaFree(scale);
}

template<size_t dim>
void scale_vec_cpp() {
	const vec<dim> v1 = init_vec<dim>();
	float scale = dist(rng);

	vec<dim> v2 = v1 * scale;
	vec<dim> res = scale * v1;

	assert(v2 == res);

	vec<dim> check_vec;
	for (size_t i = 0; i < dim; i++) {
		check_vec.data[i] = v1.data[i] * scale;
	}

	assert(v2 == check_vec);
}

template<size_t dim>
struct scale_vec {
	void operator()() {
		// Test for floating point accuracy on both CPU & GPU
		scale_vec_cpp<dim>();
		scale_vec_cu<dim>();

		// Hardcoded test for algorithm correctness
	}
};

int main() {
	run_tests<scale_vec, 2, 64>();
}

