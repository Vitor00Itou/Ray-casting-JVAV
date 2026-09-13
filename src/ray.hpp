#pragma once

#include "vec3.hpp"

/**
 * @brief Representation of a 3D ray with origin and normalized direction.
 */
struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray() : origin(0.0f, 0.0f, 0.0f), direction(0.0f, 0.0f, -1.0f) {}
    Ray(const Vec3& origin, const Vec3& direction)
        : origin(origin), direction(direction.normalize()) {}
};

