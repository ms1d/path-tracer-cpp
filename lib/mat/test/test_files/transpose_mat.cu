#include "mat.cuh"
#include "../test_runner.h"
#include <cassert>


template<size_t r, size_t c>
__global__ void transpose_mat_kernel(const mat<r,c> *m, mat<c,r> *res) {
	*res = m->transpose();
}

template<size_t r, size_t c>
void transpose_mat_cu() {
	mat<r,c> *m;
	mat<c,r> *res;

	cudaMallocManaged(&m, sizeof(mat<r,c>));
	cudaMallocManaged(&res, sizeof(mat<c,r>));

	*m = init_mat<r,c>();

	transpose_mat_kernel<<<1,1>>>(m, res);
	cudaDeviceSynchronize();

	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			assert(m->data[i][j] == res->data[j][i]);
		}
	}

	cudaFree(m);
	cudaFree(res);
}

template<size_t r, size_t c>
void transpose_mat_cpp() {
	mat<r,c> m = init_mat<r,c>();
	mat<c,r> res = m.transpose();
	
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			assert(m.data[i][j] == res.data[j][i]);
		}
	}
}

template<size_t r1, size_t c1, size_t r2, size_t c2>
struct transpose_mat {
	void operator()() {
		// Test for floating point accuracy on both CPU & GPU
		transpose_mat_cpp<r1,c1>();
		transpose_mat_cu<r1,c1>();
	
		// Hardcoded test for algorithm correctness
		transpose_mat_example();
	}

	void transpose_mat_example() {
		mat<3,2> m;
		mat<2,3> res;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 2; j++) {
				m.data[i][j] = 2 * i + j;
			}
		}

		res = m.transpose();

		assert(res.data[0][0] == 0);
		assert(res.data[1][0] == 1);
		assert(res.data[0][1] == 2);
		assert(res.data[1][1] == 3);
		assert(res.data[0][2] == 4);
		assert(res.data[1][2] == 5);

	}
};

int main() {
	run_tests<transpose_mat, 2, 16, 2, 16, 2, 2, 2, 2>();
}
