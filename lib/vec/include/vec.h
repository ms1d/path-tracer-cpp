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
			data[0] = data[1]*other.data[2] - data[2]*other.data[1];
			data[1] = data[2]*other.data[0] - data[0]*other.data[2];
			data[2] = data[0]*other.data[1] - data[1]*other.data[0];
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
