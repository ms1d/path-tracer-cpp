#include "../test_runner.h"
#include "matrix.cuh"
#include <cassert>

template<size_t r, size_t c>
__global__ void scale_matrix_kernel(const matrix<r,c>* m, const float* scalar, matrix<r,c>* res) {
	*res = *scalar * *m;
}

template<size_t r, size_t c>
void scale_matrix_cu() {
	matrix<r,c> *m, *res;
	float *scalar;

	cudaMallocManaged(&m, sizeof(matrix<r,c>));
	cudaMallocManaged(&res, sizeof(matrix<r,c>));
    cudaMallocManaged(&scalar, sizeof(float));

	*m = init_matrix<r,c>();
	*scalar = dist(rng);

	scale_matrix_kernel<<<1,1>>>(m, scalar, res);
	cudaDeviceSynchronize();
	
	matrix<r,c> check_matrix;
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			check_matrix.data[i][j] = m->data[i][j] * *scalar;
		}
	}

	assert(check_matrix == *res && *res == *scalar * *m);

	cudaFree(m);
	cudaFree(res);
	cudaFree(scalar);
}

template<size_t r, size_t c>
void scale_matrix_cpp() {
	const matrix<r,c> m = init_matrix<r,c>();
	const float scalar = dist(rng);

	matrix<r,c> res = m * scalar;

	matrix<r,c> check_matrix;
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			check_matrix.data[i][j] = m.data[i][j] * scalar;
		}
	}

	assert(check_matrix == res && res == scalar * m);
}

template<size_t r1, size_t c1, size_t r2, size_t c2>
struct scale_matrix {
	void operator()() {
		// Test for floating point accuracy on both CPU & GPU
		scale_matrix_cpp<r1, c1>();
		scale_matrix_cu<r1, c1>();
		
		// Hardcoded test for algorithm correctness
		scale_matrix_example();
	}

	void scale_matrix_example() {
        matrix<3,3> m;
        constexpr float scalar = 2.0f;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				m.data[i][j] = i + j;
			}
		}

		matrix<3,3> res1 = m * scalar, res2 = scalar * m;

		assert(res1 == res2);
		assert(res1.data[0][0] == 0);
		assert(res1.data[0][1] == 2);
		assert(res1.data[0][2] == 4);
		assert(res1.data[1][0] == 2);
		assert(res1.data[1][1] == 4);
		assert(res1.data[1][2] == 6);
		assert(res1.data[2][0] == 4);
		assert(res1.data[2][1] == 6);
		assert(res1.data[2][2] == 8);
	}
};

int main() {
	run_tests<scale_matrix, 2, 16, 2, 16, 2, 2, 2, 2>();
}

