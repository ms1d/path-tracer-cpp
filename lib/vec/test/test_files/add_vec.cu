#include "../test_runner.h"
#include <cassert>
#include "vec.h"

template<size_t dim>
__global__ void add_vec_kernel(const vec<dim>* v1, const vec<dim>* v2, vec<dim>* v3) {
	*v3 = *v1 + *v2;
}

template<size_t dim>
void add_vec_cu() {
	vec<dim> *v1, *v2, *v3;

	cudaMallocManaged(&v1, sizeof(vec<dim>));
	cudaMallocManaged(&v2, sizeof(vec<dim>));
	cudaMallocManaged(&v3, sizeof(vec<dim>));

	*v1 = init_vec<dim>();
    *v2 = init_vec<dim>();

	add_vec_kernel<<<1, 1>>>(v1, v2, v3);
	cudaDeviceSynchronize();

	assert(*v3 == *v1 + *v2);

	cudaFree(v1);
	cudaFree(v2);
    cudaFree(v3);
}

template<size_t dim>
void add_vec_cpp() {
	const vec<dim> v1 = init_vec<dim>(), v2 = init_vec<dim>();

	vec<dim> v3 = v1 + v2;

	for (size_t i = 0; i < dim; ++i)
		assert(v3.data[i] == v1.data[i] + v2.data[i]);
}

template<size_t dim>
struct add_vec {
    void operator()() const {
		add_vec_cpp<dim>();
		add_vec_cu<dim>();
    }

};

int main() {
    run_tests<add_vec, 2, 64>();
}
