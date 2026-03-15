#pragma once

#include <iostream> // IWYU pragma: keep
#include <cmath>

#ifndef __host__
#define __host__
#endif
#ifndef __device__
#define __device__
#endif

// Must implement:
// 1 - vector addition and subtraction + assignment operators
// 2 - vector-scalar multiplication + assignment operators
// 3 - dot product
// 4 - cross product + assignment operator
// 5 - is equal
template<size_t dim>
struct vec {
	public:
		static_assert(dim > 1, "Vector dimensions must be > 1");
		float data[dim];



		__host__ __device__ vec() { }

		__host__ __device__ vec(const float (&new_data)[dim]) {
			for (int i = 0; i < dim; i++) {
                data[i] = new_data[i];
            }
		}



		__host__ __device__ float GetMagnitude() const {
			float sum = 0;
			for (const float& axis : data) {
				sum += axis * axis;
			}

			return sqrt(sum);
		}



		__host__ __device__ vec operator+(const vec& other) const {
			vec<dim> v = *this;
			v += other;
			return v;
		}

		__host__ __device__ vec& operator+=(const vec& other) {
			for (int i = 0; i < dim; i++) {
				data[i] += other.data[i];	
			}

			return *this;
		}



		__host__ __device__ vec operator-(const vec& other) const {
			vec<dim> v = *this;
			v -= other;
			return v;
		}

		__host__ __device__ vec& operator-=(const vec& other) {
			for (int i = 0; i < dim; i++) {
				data[i] -= other.data[i];	
			}

			return *this;
		}	



		__host__ __device__ bool operator==(const vec& other) const {
			for (int i = 0; i < dim; i++) {
				if (data[i] != other.data[i]) { return false; }
			}

			return true;
		}



		// Cross product (3D only)
		__host__ __device__ vec<3> operator^(const vec<3>& other) const {
			vec<3> res = *this;
			res ^= other;
			return res;
		}

		__host__ __device__ vec<3>& operator^=(const vec<3>& other) {
			float x = data[0], y = data[1], z = data[2];
			data[0] = y*other.data[2] - z*other.data[1];
			data[1] = z*other.data[0] - x*other.data[2];
			data[2] = x*other.data[1] - y*other.data[0];
			return *this;
		}



		// Dot product
		__host__ __device__ float operator*(const vec& other) const {
			float sum = 0;

			for (int i = 0; i < dim; i++) {
				sum += data[i] * other.data[i];
			}

			return sum;
		}



		// Scalar product
		__host__ __device__ friend vec operator*(vec v, float scalar) {
			v *= scalar;
			return v;
		}

		__host__ __device__ friend vec operator*(float scalar, vec v) {
			v *= scalar;
            return v;
        }

		__host__ __device__ vec& operator*=(float scalar) {
			for (float& axis : data) {
				axis *= scalar;
			}

			return *this;
		}



		// ms1d: clangd lsp in neovim is throwing errors
		// due to the ostream so suppressing lsp output here
#ifndef __CUDA_ARCH__
		friend std::ostream& operator<<(std::ostream& os, const vec& v) {
			os << "(";

			const char* sep = "";
			for (const float& axis : v.data) {
				os << sep << axis;
				sep = ", ";
			}

			return os << ")";
		}
#endif
};
