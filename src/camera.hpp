#pragma once

#include "vec3.hpp"
#include "ray.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Perspective Camera computing viewing rays given screen coordinates (u, v).
 */
struct Camera {
    Vec3 position;
    Vec3 forward;
    Vec3 up;
    float fov = 45.0f;
    float aspectRatio = 800.0f / 600.0f;

    Camera() : position(0, 0, 0), forward(0, 0, -1), up(0, 1, 0), fov(45.0f) {}

    Camera(Vec3 pos, Vec3 fwd, Vec3 upDir, float fovDeg, float aspect = 800.0f / 600.0f)
        : position(pos), forward(fwd.normalize()), up(upDir.normalize()), fov(fovDeg), aspectRatio(aspect) {}

    /**
     * @brief Generate a primary ray for viewport normalized UV coordinates [0.0, 1.0].
     */
    Ray getRay(float u, float v) const {
        Vec3 right = forward.cross(up).normalize();
        Vec3 correctedUp = right.cross(forward).normalize();

        float scale = std::tan(fov * 0.5f * static_cast<float>(M_PI) / 180.0f);

        Vec3 rayDir = (forward
                     + right * ((2.0f * u - 1.0f) * aspectRatio * scale)
                     - correctedUp * ((2.0f * v - 1.0f) * scale)).normalize();

        return Ray(position, rayDir);
    }
};
