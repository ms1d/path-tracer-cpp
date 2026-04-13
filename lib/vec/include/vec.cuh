#pragma once



#ifndef __host__
#define __host__
#endif
#ifndef __device__
#define __device__
#endif



#include <iostream> 
#include "precision.cuh"



// Generic vector. Implements:
//
//		- Addition + Addition Assignment
//		- Subtraction + Subtraction Assignment
//
//		- Scalar multiplication + assignment operators
//		- Vector multiplication (Dot product)
//
//		- Equality test (with strict tolerance)
//
//		vec_base is a base class that allows for more readable aliases (via union)
//		for specialised vectors (e.g. 2D x and y, 3D x, y and z, 4D x, y, z and w...)
//		see vec<3> below

template<size_t dim, class Derived>
struct vec_base {



    static_assert(dim > 1, "Vector dimensions must be > 1");



    __host__ __device__ constexpr float* derived_data() { return static_cast<Derived*>(this)->data; }
    __host__ __device__ constexpr const float* derived_data() const { return static_cast<const Derived*>(this)->data; }



	__host__ __device__ constexpr vec_base() {}
    __host__ __device__ constexpr vec_base(const float (&new_data)[dim]) {
        float* d = derived_data();
        for (size_t i = 0; i < dim; i++) d[i] = new_data[i];
    }



	__host__ __device__ constexpr float operator[](size_t i) const {
		return derived_data()[i];
	}



    __host__ __device__ constexpr float mag() const {
        const float* d = derived_data();
        float sum = 0;
        for (size_t i = 0; i < dim; i++) sum += d[i] * d[i];
        return __builtin_sqrtf(sum);
    }



	__host__ __device__ constexpr Derived norm() const {
		Derived res = static_cast<const Derived&>(*this);
		res.norm_inplace();
		return res;
	}



	__host__ __device__ constexpr Derived& norm_inplace() {
		float magnitude = mag();
		auto data = derived_data();
		for (size_t i = 0; i < dim; i++) {
			data[i] /= magnitude;
		}
		return static_cast<Derived&>(*this);
	}


    __host__ __device__ constexpr Derived operator+(const Derived& other) const {
        Derived res = static_cast<const Derived&>(*this);
        res += other;
        return res;
    }
    
	__host__ __device__ constexpr Derived& operator+=(const Derived& other) {
        float* d = derived_data();
        for (size_t i = 0; i < dim; i++) d[i] += other.data[i];
        return static_cast<Derived&>(*this);
    }



    __host__ __device__ constexpr Derived operator-(const Derived& other) const {
        Derived v = static_cast<const Derived&>(*this);
        v -= other;
        return v;
    }
    
	__host__ __device__ constexpr Derived& operator-=(const Derived& other) {
        float* d = derived_data();
        for (size_t i = 0; i < dim; i++) d[i] -= other.data[i];
        return static_cast<Derived&>(*this);
    }



    __host__ __device__ constexpr float operator*(const Derived& other) const { // dot
        const float* d = derived_data();
        float sum = 0;
        for (size_t i = 0; i < dim; i++) sum += d[i] * other.data[i];
        return sum;
    }



    __host__ __device__ constexpr Derived& operator*=(float scalar) {
        float* d = derived_data();
        for (size_t i = 0; i < dim; i++) d[i] *= scalar;
        return static_cast<Derived&>(*this);
    }



	__host__ __device__ constexpr bool operator==(const vec_base<dim, Derived>& rhs) const {
		const float* ld = derived_data();
		const float* rd = rhs.derived_data();

		for (size_t i = 0; i < dim; i++) if (!math_precision::nearly_equal(ld[i], rd[i])) return false;
		return true;
	}



	__host__ __device__ constexpr Derived operator*(float scalar) const {
		Derived result = static_cast<const Derived&>(*this);
		result *= scalar;
		return result;
	}



};




template<size_t dim>
struct vec : vec_base<dim, vec<dim>> {



	float data[dim];



};




// float * scalar must be a non-member function to allow float on lhs
template<size_t dim>
__host__ __device__ constexpr vec<dim> operator*(float scalar, const vec<dim>& v) { return v * scalar; }



template<size_t dim> 
constexpr std::ostream& operator<<(std::ostream& os, const vec<dim>& v) {
	const float* d = v.data;
	os << "(";
	const char* sep = "";
	for (size_t i = 0; i < dim; i++) {
		os << sep << d[i];
		sep = ", ";
	}
	return os << ")";
}



// 3D vector. Implements:
//		- Cross product + assignment operator
//		- clean x,y,z aliases
template<>
struct vec<3> : vec_base<3, vec<3>> {



	using base = vec_base<3, vec<3>>;



    union {
        float data[3];
        struct { float x, y, z; };
    };




    __host__ __device__ constexpr vec() {}

    __host__ __device__ constexpr vec(float x, float y, float z) : x(x), y(y), z(z) {}

	__host__ __device__ constexpr vec(const vec& other) : x(other.x), y(other.y), z(other.z) {}

    __host__ __device__ constexpr vec& operator=(const vec& other) {
		x = other.x; y = other.y; z = other.z;
		return *this;
	}




    // Cross product
    __host__ __device__ constexpr vec operator^(const vec& other) const {
        vec res = *this;
        res ^= other;
        return res;
    }
    __host__ __device__ constexpr vec& operator^=(const vec& other) {
        float _x = x, _y = y, _z = z;
        x = _y * other.z - _z * other.y;
        y = _z * other.x - _x * other.z;
        z = _x * other.y - _y * other.x;
        return *this;
    }



};
