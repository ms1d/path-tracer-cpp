#pragma once



#include "vec.cuh"
#include <utility>



#ifndef __host__
#define __host__
#endif
#ifndef __device__
#define __device__
#endif



// Generic r by c matrix. Supports:
//
//		- Addition + Addition assignment
//		- Subtraction + Subtraction assignment
//
//		- Matrix Multiplication + Multiplication assignment
//		- Vector Multiplication/Transformation (mat * vec only)
//		- Scalar Multiplication (mat * scalar + assignment, scalar * mat no assignment)
//
//		- Tranposition + Transposition assignment (transpose_inplace)
//
//		- Determinant (simple Laplace expansion, O(n!), DO NOT USE FOR LARGE MATRICES!)
//
//		- Generating minor matrices (row to leave out, collumn to leave out)
//
//		- Equality test (with 2e-6 tolerance)
//
//		- (TBI) Inverse

template <size_t r, size_t c>
struct mat {



	float data[r][c];



	__host__ __device__ mat() {}



	__host__ __device__ constexpr mat& operator+=(const mat& other) {
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < c; j++) {
				data[i][j] += other.data[i][j];
			}
		}

		return *this;
	}

	__host__ __device__ constexpr mat operator+(const mat& other) const {
		mat res = *this;
		res += other;
		return res;
	}



	__host__ __device__ constexpr mat& operator-=(const mat& other) {
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < c; j++) {
				data[i][j] -= other.data[i][j];
			}
		}

		return *this;
	}

	__host__ __device__ constexpr mat operator-(const mat& other) const {
		mat res = *this;
		res -= other;
		return res;
	}



	__host__ __device__ constexpr mat<r-1, c-1> get_minor(int row, int col) const requires(r > 1 && c > 1) {
		mat<r-1,c-1> res;
		int curr_row = 0;

		for (int i = 0; i < r; i++) {
			if (i == row) continue;
			int curr_col = 0;

			for (int j = 0; j < c; j++) {
				if (j == col) continue;

				res.data[curr_row][curr_col] = data[i][j];

				curr_col++;
			}

			curr_row++;
		}

		return res;
	}



	__host__ __device__ constexpr float det() const requires(r == c && r == 2) { return data[0][0] * data[1][1] - data[0][1] * data[1][0]; }
	__host__ __device__ constexpr float det() const requires(r == c && r > 2){
		float det = 0;

		int sign = 1;
		for (int j = 0; j < c; j++) {
			mat<r-1,c-1> minor = get_minor(0, j);
			det += sign * data[0][j] * minor.det();
			sign *= -1;
		}

		return det;
	};



	__host__ __device__ constexpr mat transpose_inplace() requires(r == c) {
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < c; j++) {
				std::swap(data[i][j], data[j][i]);
			}
		}

		return *this;
	}



	__host__ __device__ constexpr mat<c,r> transpose() const {
		mat<c,r> res;

		for (int i = 0; i < c; i++) {
			for (int j = 0; j < r; j++) {
				res.data[i][j] = data[j][i];
			}
		}

		return res;
	}


	__host__ __device__ constexpr mat& operator*=(float scalar) {
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < c; j++) {
				data[i][j] *= scalar;
			}
		}

		return *this;
	}

	__host__ __device__ constexpr mat operator*(float scalar) const {
		mat m = *this;
		m *= scalar;
		return m;
	}



	__host__ __device__ constexpr vec<r> operator*(const vec<c>& v) const {
		vec<r> res;

		for (size_t i = 0; i < r; ++i) {
			float sum = 0;
			for (size_t j = 0; j < c; ++j)
				sum += data[i][j] * v.data[j];
			res.data[i] = sum;
		}

		return res;
	}


	__host__ __device__ constexpr bool operator==(const mat& other) const {
		constexpr float epsilon = 2e-6;

		for (int i = 0; i < r; i++) {
			for (int j = 0; j < c; j++) {
				if (fabs(data[i][j] - other.data[i][j]) > epsilon) return false;
			}
		}

		return true;
	}



};

template <size_t r, size_t c>
__host__ __device__ constexpr mat<r, c> operator*(float scalar, const mat<r, c>& m) {
	return m * scalar;
}

template<size_t r1, size_t r2, size_t c1, size_t c2>
__host__ __device__ constexpr mat<r1, c2> operator*(const mat<r1, c1>& m1, const mat<r2, c2>& m2) requires(c1 == r2) {
	mat<r1,c2> res;

	for (size_t i = 0; i < r1; ++i) {
		for (size_t j = 0; j < c2; ++j) {
			float sum = 0;

			for (size_t k = 0; k < c1; ++k)
				sum += m1.data[i][k] * m2.data[k][j];

			res.data[i][j] = sum;
		}
	}

	return res;
}
