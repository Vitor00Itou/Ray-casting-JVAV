// ray.hpp
#pragma once
#include "vec3.hpp"

struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray() : origin(Vec3(0,0,0)), direction(Vec3(0,0,0)) {}
    Ray(const Vec3& origin, const Vec3& direction)
        : origin(origin), direction(direction.normalize()) {}
};
