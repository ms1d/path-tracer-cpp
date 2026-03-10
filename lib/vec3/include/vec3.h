#pragma once

#define VEC3_H

struct vec3 {
	public:
		float x, y, z;

		vec3(float x, float y, float z);

		float GetMagnitude() const;

		vec3 operator+(const vec3& other) const;
		
		vec3 operator-(const vec3& other) const;

		bool operator==(const vec3& other) const;

		// Cross product
		vec3 operator^(const vec3& other) const;

		// Dot product
		float operator*(const vec3& other) const;
};
