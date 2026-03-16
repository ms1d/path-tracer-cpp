#include "../test_runner.h"
#include "matrix.cuh"
#include <cassert>

template<size_t r, size_t c>
__global__ void sub_matrix_kernel(const matrix<r,c> *m1, const matrix<r,c> *m2, matrix<r,c> *res) {
	*res = *m1 - *m2;
}

template<size_t r, size_t c>
void sub_matrix_cu() {
	matrix<r,c> *m1, *m2, *res;

	cudaMallocManaged(&m1, sizeof(matrix<r,c>));
	cudaMallocManaged(&m2, sizeof(matrix<r,c>));
	cudaMallocManaged(&res, sizeof(matrix<r,c>));

	*m1 = init_matrix<r,c>();
    *m2 = init_matrix<r,c>();

	sub_matrix_kernel<<<1,1>>>(m1, m2, res);
	cudaDeviceSynchronize();

	assert(*res == *m1 - *m2);

	cudaFree(m1);
    cudaFree(m2);
	cudaFree(res);
}

template<size_t r, size_t c>
void sub_matrix_cpp() {
	const matrix<r,c> m1 = init_matrix<r,c>(), m2 = init_matrix<r,c>();
	matrix<r,c> res = m1 - m2;

	assert(res == m1 - m2);
}

template<size_t r1, size_t c1, size_t r2, size_t c2>
struct sub_matrix {
	void operator()() {
		// Test for floating point accuracy on both CPU & GPU
		sub_matrix_cpp<r1, c1>();
		sub_matrix_cu<r1, c1>();

		// Hardcoded test for algorithm correctness
		sub_matrix_example();
	}

	void sub_matrix_example() {
		matrix<3,3> m1, m2;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				m1.data[i][j] = i + j;
				m2.data[i][j] = i * j;
			}
		}

		matrix<3,3> res1 = m1 - m2, res2 = m2 - m1;

		assert(res1 == -1 * res2);
		assert(res1.data[0][0] == 0);
		assert(res1.data[0][1] == 1);
		assert(res1.data[0][2] == 2);
		assert(res1.data[1][0] == 1);
		assert(res1.data[1][1] == 1);
		assert(res1.data[1][2] == 1);
		assert(res1.data[2][0] == 2);
		assert(res1.data[2][1] == 1);
		assert(res1.data[2][2] == 0);
	}
};

int main() {
	run_tests<sub_matrix, 2, 16, 2, 16, 2, 2, 2, 2>();
}

