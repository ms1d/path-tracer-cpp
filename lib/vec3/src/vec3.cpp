#include <cmath>
#include "vec3.h"

vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}

float vec3::GetMagnitude() const {
    return std::sqrt(x*x + y*y + z*z);
}

vec3 vec3::operator+(const vec3& other) const {
    return vec3{x + other.x, y + other.y, z + other.z};
}

vec3 vec3::operator-(const vec3& other) const {
    return vec3{x - other.x, y - other.y, z - other.z};
}

bool vec3::operator==(const vec3& other) const {
    return x == other.x && y == other.y && z == other.z;
}

vec3 vec3::operator^(const vec3& other) const {
    return vec3{
        y*other.z - z*other.y,
        z*other.x - x*other.z,
        x*other.y - y*other.x
    };
}

float vec3::operator*(const vec3& other) const {
    return x*other.x + y*other.y + z*other.z;
}
