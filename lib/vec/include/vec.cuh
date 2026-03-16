#pragma once

#include <iostream> // IWYU pragma: keep
#include <cmath>

#ifndef __host__
#define __host__
#endif
#ifndef __device__
#define __device__
#endif

// Implements:
// 1 - vector addition and subtraction + assignment operators
// 2 - vector-scalar multiplication + assignment operators
// 3 - dot product
// 4 - cross product + assignment operator
// 5 - is equal
// vec_base is a base class that allows for more readable aliases
// for specialised vectors (e.g. 2D x and y, 3D x, y and z, 4D x, y, z and w...)
template<size_t dim, class Derived>
struct vec_base {
    static_assert(dim > 1, "Vector dimensions must be > 1");



    __host__ __device__ float* derived_data() { return static_cast<Derived*>(this)->data; }
    __host__ __device__ const float* derived_data() const { return static_cast<const Derived*>(this)->data; }

    

	__host__ __device__ vec_base() {}
    __host__ __device__ vec_base(const float (&new_data)[dim]) {
        float* d = derived_data();
        for (size_t i = 0; i < dim; i++) d[i] = new_data[i];
    }



	__host__ __device__ float operator[](int i) const {
		return derived_data()[i];
	}



    __host__ __device__ float GetMagnitude() const {
        const float* d = derived_data();
        float sum = 0;
        for (size_t i = 0; i < dim; i++) sum += d[i] * d[i];
        return sqrt(sum);
    }



    __host__ __device__ Derived operator+(const Derived& other) const {
        Derived v = static_cast<const Derived&>(*this);
        v += other;
        return v;
    }
    
	__host__ __device__ Derived& operator+=(const Derived& other) {
        float* d = derived_data();
        for (size_t i = 0; i < dim; i++) d[i] += other.data[i];
        return static_cast<Derived&>(*this);
    }



    __host__ __device__ Derived operator-(const Derived& other) const {
        Derived v = static_cast<const Derived&>(*this);
        v -= other;
        return v;
    }
    
	__host__ __device__ Derived& operator-=(const Derived& other) {
        float* d = derived_data();
        for (size_t i = 0; i < dim; i++) d[i] -= other.data[i];
        return static_cast<Derived&>(*this);
    }



    __host__ __device__ float operator*(const Derived& other) const { // dot
        const float* d = derived_data();
        float sum = 0;
        for (size_t i = 0; i < dim; i++) sum += d[i] * other.data[i];
        return sum;
    }

    __host__ __device__ Derived& operator*=(float scalar) {
        float* d = derived_data();
        for (size_t i = 0; i < dim; i++) d[i] *= scalar;
        return static_cast<Derived&>(*this);
    }



#ifndef __CUDA_ARCH__
    friend std::ostream& operator<<(std::ostream& os, const Derived& v) {
        const float* d = v.derived_data();
        os << "(";
        const char* sep = "";
        for (size_t i = 0; i < dim; i++) {
            os << sep << d[i];
            sep = ", ";
        }
        return os << ")";
    }
#endif
};

// Scalar multiplication
template<size_t dim, class Derived>
__host__ __device__ Derived operator*(float scalar, const vec_base<dim, Derived>& v) {
    Derived result = static_cast<const Derived&>(v);
    result *= scalar;
    return result;
}

template<size_t dim, class Derived>
__host__ __device__ Derived operator*(const vec_base<dim, Derived>& v, float scalar) {
    Derived result = static_cast<const Derived&>(v);
    result *= scalar;
    return result;
}

template<size_t dim, class Derived>
__host__ __device__ bool operator==(const vec_base<dim, Derived>& lhs, const vec_base<dim, Derived>& rhs) {
    const float* ld = lhs.derived_data();
    const float* rd = rhs.derived_data();
    constexpr float epsilon = 2e-6f;

    for (size_t i = 0; i < dim; i++) if (fabs(ld[i] - rd[i]) > epsilon) return false;
    return true;
}

template<size_t dim>
struct vec : vec_base<dim, vec<dim>> {
	float data[dim];
};



template<>
struct vec<3> : vec_base<3, vec<3>> {
    union {
        float data[3];
        struct { float x, y, z; };
    };

    __host__ __device__ vec() {}

    __host__ __device__ vec(float x, float y, float z) : x(x), y(y), z(z) {}

	__host__ __device__ vec(const vec& other) : x(other.x), y(other.y), z(other.z) {}

    __host__ __device__ vec& operator=(const vec& other) {
		x = other.x; y = other.y; z = other.z;
		return *this;
	}

    // Cross product
    __host__ __device__ vec operator^(const vec& other) const {
        vec res = *this;
        res ^= other;
        return res;
    }
    __host__ __device__ vec& operator^=(const vec& other) {
        float _x = x, _y = y, _z = z;
        x = _y * other.z - _z * other.y;
        y = _z * other.x - _x * other.z;
        z = _x * other.y - _y * other.x;
        return *this;
    }
};

// Hint for clangd LSP to resolve vec<3> comparison cleanly
// Compiler will optimise this out
__host__ __device__ inline bool operator==(const vec<3>& lhs, const vec<3>& rhs) {
    return static_cast<const vec_base<3, vec<3>>&>(lhs) == static_cast<const vec_base<3, vec<3>>&>(rhs);
}
