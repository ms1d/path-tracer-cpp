#include "../test_runner.h"
#include "matrix.cuh"
#include <cassert>

template<size_t r1, size_t c1, size_t r2, size_t c2>
__global__ void mult_matrix_kernel(const matrix<r1,c1>* m1, const matrix<r2,c2>* m2, matrix<r1,c2>* res) {
	if constexpr (c1 != r2) return;
	*res = *m1 * *m2;
}

template<size_t r1, size_t c1, size_t r2, size_t c2>
void mult_matrix_cu() {
	if constexpr (c1 != r2) return;
	matrix<r1,c1> *m1;
	matrix<r2,c2> *m2;
	matrix<r1,c2> *res;

	cudaMallocManaged(&m1, sizeof(matrix<r1,c1>));
	cudaMallocManaged(&m2, sizeof(matrix<r2,c2>));
	cudaMallocManaged(&res, sizeof(matrix<r1,c2>));

	*m1 = init_matrix<r1,c1>();
    *m2 = init_matrix<r2,c2>();

	mult_matrix_kernel<<<1,1>>>(m1, m2, res);
	cudaDeviceSynchronize();

	matrix<r1,c2> check_matrix;

	for (int i = 0; i < r1; i++) {
		for (int j = 0; j < c2; j++) {
			float sum = 0;

			for (int k = 0; k < c1; k++) {
				sum += m1->data[i][k] * m2->data[k][j];
			}

			check_matrix.data[i][j] = sum;
		}
	}

	assert(check_matrix == *res);
	
	cudaFree(m1);
	cudaFree(m2);
	cudaFree(res);
}

template<size_t r1, size_t c1, size_t r2, size_t c2>
void mult_matrix_cpp() {
	if constexpr (c1 != r2) return;

	const matrix<r1,c1> m1 = init_matrix<r1,c1>();
	const matrix<r2,c2> m2 = init_matrix<r2,c2>();

	matrix<r1,c2> res = m1 * m2, check_matrix;

	for (int i = 0; i < r1; i++) {
		for (int j = 0; j < c2; j++) {
			float sum = 0;

			for (int k = 0; k < c1; k++) {
				sum += m1.data[i][k] * m2.data[k][j];
			}

			check_matrix.data[i][j] = sum;
		}
	}

	assert(check_matrix == res);
}

template<size_t r1, size_t c1, size_t r2, size_t c2>
struct mult_matrix {
	void operator()() {
		if constexpr (c1 == r2) {
			// Test for floating point accuracy on both CPU & GPU
			mult_matrix_cpp<r1, c1, r2, c2>();
			mult_matrix_cu<r1, c1, r2, c2>();

			// Hardcoded test for algorithm correctness
			mult_matrix_example();
		}
	}

	void mult_matrix_example() {
		matrix<3,3> m1, m2;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				m1.data[i][j] = i + j;
				m2.data[i][j] = i * j;
			}
		}

		matrix<3,3> res1 = m1 * m2, res2 = m2 * m1;

		assert(res1 != res2);
		assert(res1.data[0][0] == 0), assert(res2.data[0][0] == 0);
		assert(res1.data[0][1] == 5), assert(res2.data[0][1] == 0);
		assert(res1.data[0][2] == 10), assert(res2.data[0][2] == 0);
		assert(res1.data[1][0] == 0), assert(res2.data[1][0] == 5);
		assert(res1.data[1][1] == 8), assert(res2.data[1][1] == 8);
		assert(res1.data[1][2] == 16), assert(res2.data[1][2] == 11);
		assert(res1.data[2][0] == 0), assert(res2.data[2][0] == 10);
		assert(res1.data[2][1] == 11), assert(res2.data[2][1] == 16);
		assert(res1.data[2][2] == 22), assert(res2.data[2][2] == 22);

	}
};

int main() {
	run_tests<mult_matrix, 2, 16, 2, 16, 2, 16, 2, 16>();
}

