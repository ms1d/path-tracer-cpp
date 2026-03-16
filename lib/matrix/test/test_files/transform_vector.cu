#include "../test_runner.h"
#include "matrix.cuh"
#include "vec.cuh"
#include <cassert>

template<size_t r, size_t c>
__global__ void transform_vector_kernel(const matrix<r,c>* m, const vec<c>* v, vec<r>* res) {
	*res = *m * *v;
}

template<size_t r, size_t c>
void transform_vector_cu() {
	matrix<r,c> *m;
	vec<c> *v;
	vec<r> *res;

	cudaMallocManaged(&m, sizeof(matrix<r,c>));
    cudaMallocManaged(&v, sizeof(vec<c>));
	cudaMallocManaged(&res, sizeof(vec<r>));

	*m = init_matrix<r,c>();
	*v = init_vec<c>();

	transform_vector_kernel<<<1,1>>>(m, v, res);
    cudaDeviceSynchronize();

	vec<r> check_vec;
	for (int i = 0; i < r; i++) {
		float sum = 0;
		for (int j = 0; j < c; j++) {
			sum += v->data[j] * m->data[i][j];
		}

		check_vec.data[i] = sum;
	}

	assert(*res == check_vec);
	cudaFree(m);
	cudaFree(v);
	cudaFree(res);

}

template<size_t r, size_t c>
void transform_vector_cpp() {
	const matrix<r,c> m = init_matrix<r,c>();
	const vec<c> v = init_vec<c>();

	vec<r> res = m * v;

	vec<r> check_vec;
	for (int i = 0; i < r; i++) {
		float sum = 0;
		for (int j = 0; j < c; j++) {
			sum += v.data[j] * m.data[i][j];
		}

		check_vec.data[i] = sum;
	}

	assert(res == check_vec);
}

template<size_t r1, size_t c1, size_t r2, size_t c2>
struct transform_vector {
	void operator()() {
		// Test for floating point accuracy on both CPU & GPU
		transform_vector_cpp<r1, c1>();
		transform_vector_cu<r1, c1>();

		// Hardcoded test for algorithm correctness
		transform_vector_example();
	}

	void transform_vector_example() {
		matrix<3,3> m;
		const vec<3> v{0, 1, 2};

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				m.data[i][j] = i + j;
			}
		}

		vec<3> res = m * v;
		assert(res.x == 5);
		assert(res.y == 8);
		assert(res.z == 11);
	}
};

int main() {
	run_tests<transform_vector, 2, 16, 2, 16, 2, 2, 2, 2>();
}

