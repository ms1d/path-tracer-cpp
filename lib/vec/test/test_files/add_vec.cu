#include "../test_runner.h"
#include <cassert>
#include "vec.cuh"

template<size_t dim>
__global__ void add_vec_kernel(const vec<dim>* v1, const vec<dim>* v2, vec<dim>* res) {
	*res = *v1 + *v2;
}

template<size_t dim>
void add_vec_cu() {
	vec<dim> *v1, *v2, *res;

	cudaMallocManaged(&v1, sizeof(vec<dim>));
	cudaMallocManaged(&v2, sizeof(vec<dim>));
	cudaMallocManaged(&res, sizeof(vec<dim>));

	*v1 = init_vec<dim>();
    *v2 = init_vec<dim>();

	add_vec_kernel<<<1, 1>>>(v1, v2, res);
	cudaDeviceSynchronize();

	vec<dim> check_vec;
	for (int i = 0; i < dim; i++) {
		check_vec.data[i] = v1->data[i] + v2->data[i];
	}

	assert(*res == check_vec);

	cudaFree(v1);
	cudaFree(v2);
    cudaFree(res);
}

template<size_t dim>
void add_vec_cpp() {
	const vec<dim> v1 = init_vec<dim>(), v2 = init_vec<dim>();

	vec<dim> res = v1 + v2;
	
	vec<dim> check_vec;
	for (int i = 0; i < dim; i++) {
		check_vec.data[i] = v1.data[i] + v2.data[i];
	}

	assert(res == check_vec);
}

template<size_t dim>
struct add_vec {
    void operator()() const {
		// Test for floating point accuracy on both CPU & GPU
		add_vec_cpp<dim>();
		add_vec_cu<dim>();

		// Hardcoded test for algorithm correctness
    }

};

int main() {
    run_tests<add_vec, 2, 64>();
}
