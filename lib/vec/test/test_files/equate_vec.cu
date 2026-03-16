#include "vec.cuh"
#include "../test_runner.h"
#include <cassert>

template<size_t dim>
__global__ void equate_vec_kernel(const vec<dim>* v1, const vec<dim>* v2, int* isEqual) {
	*isEqual = *v1 == *v2 ? 1 : 0;
}

template<size_t dim>
void equate_vec_cu() {
	vec<dim> *v1, *v2;
	int *isEqual;

	cudaMallocManaged(&v1, sizeof(vec<dim>));
	cudaMallocManaged(&v2, sizeof(vec<dim>));
	cudaMallocManaged(&isEqual, sizeof(int));

	*v1 = init_vec<dim>();
	*v2 = *v1;

	equate_vec_kernel<<<1, 1>>>(v1, v2, isEqual);
    cudaDeviceSynchronize();

	assert(*isEqual == 1);

	cudaFree(v1);
	cudaFree(v2);
	cudaFree(isEqual);
}

template<size_t dim>
void equate_vec_cpp() {
	const vec<dim> v1 = init_vec<dim>(), v2 = v1;
	bool isEqual = v1 == v2;
	assert(isEqual);
}

template<size_t dim>
struct equate_vec {
	void operator()() {
		// Test for floating point accuracy on both CPU & GPU
		equate_vec_cpp<dim>();
		equate_vec_cu<dim>();

		// Hardcoded test for algorithm correctness
	}
};

int main() {
	run_tests<equate_vec, 2, 64>();
}
