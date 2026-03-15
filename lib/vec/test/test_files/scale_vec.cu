#include "vec.h"
#include "../test_runner.h"
#include <cassert>

template<size_t dim>
__global__ void scale_vec_kernel(const vec<dim>* v1, vec<dim>* v2, vec<dim>* v3, float* scale) {
	*v2 = *v1 * *scale;
	*v3 = *scale * *v1;
}

template<size_t dim>
void scale_vec_cu() {
	vec<dim> *v1, *v2, *v3;
	float *scale;

	cudaMallocManaged(&v1, sizeof(vec<dim>));
	cudaMallocManaged(&v2, sizeof(vec<dim>));
	cudaMallocManaged(&v3, sizeof(vec<dim>));
	cudaMallocManaged(&scale, sizeof(float));

	*v1 = init_vec<dim>();
    *scale = dist(rng);

	scale_vec_kernel<<<1, 1>>>(v1, v2, v3, scale);
	cudaDeviceSynchronize();

	assert(*v2 == *v3);

	vec<dim> check_vec;
	for (size_t i = 0; i < dim; i++) {
		check_vec.data[i] = v1->data[i] * scale;
	}

	assert(*v2 == check_vec);

	cudaFree(v1);
	cudaFree(v2);
	cudaFree(v3);
	cudaFree(scale);
}

template<size_t dim>
void scale_vec_cpp() {
	const vec<dim> v1 = init_vec<dim>();
	float scale = dist(rng);

	vec<dim> v2 = v1 * scale;
	vec<dim> v3 = scale * v1;

	assert(v2 == v3);

	vec<dim> check_vec;
	for (size_t i = 0; i < dim; i++) {
		check_vec.data[i] = v1.data[i] * scale;
	}

	assert(v2 == check_vec);
}

template<size_t dim>
struct scale_vec {
	void operator()() {
		scale_vec_cpp<dim>();
	}
};

int main() {
	run_tests<scale_vec, 2, 64>();
}

