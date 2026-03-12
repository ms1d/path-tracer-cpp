#pragma once

#include "vec.h"
#ifndef __host__
#define __host__
#endif
#ifndef __device__
#define __device__
#endif

template <size_t r, size_t c>
struct matrix {
	public:
		float data[r][c];
		
		__host__ __device__ float det() const;

		__host__ __device__ matrix<r, c> operator*=(float scalar) {
			for (int i = 0; i < r; i++) {
				for (int j = 0; j < c; j++) {
					data[i][j] *= scalar;
				}
			}
		}

		friend __host__ __device__ matrix<r, c> operator*(matrix<r, c> lhs, float scalar) {
			lhs *= scalar;
			return lhs;
		}
	
		__host__ __device__ vec<c> operator*=(const vec<c>& vector) const {

		};

		__host__ __device__ vec<c> operator*(const vec<c>& vector) const;
};
