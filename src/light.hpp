#pragma once
#include "vec3.hpp"

struct Light {
	Vec3 position;
	float intensity;

	Light(Vec3 position, float intensity) : position(position), intensity(intensity) {}
};