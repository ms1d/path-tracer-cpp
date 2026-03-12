#pragma once

#include "vec3.h"

struct testResult {
	vec3* A;
	vec3* B;
	vec3* C;
	int arrLength;
};

testResult launchTest();
