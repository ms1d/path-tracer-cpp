#include "matrix.h"
#include "vec.h"

template<size_t r, size_t c>
__host__ __device__ float matrix<r, c>::det() const {
	static_assert(r == c, "Matrix is not square");

	// Implementation of generic Laplace algorithm if necessary, but probably not needed
	return 0;
}

// Implementation of specific sizes (tbd)
template<>
__host__ __device__ float matrix<2, 2>::det() const {
	return 0;
}

template<>
__host__ __device__ float matrix<3, 3>::det() const {
	return 0;
}

template<>
__host__ __device__ float matrix<4, 4>::det() const {
	return 0;
}

// Vector multiplication (transformation)
template<size_t r, size_t c>
__host__ __device__ vec<c> matrix<r, c>::operator*(const vec<c>& vector) const {
	vec<c> res;

	for (int i = 0; i < r; i++) {
		vec<c> row(data[i]);
		res[i] = vector * row;
	}

	return res;
}
