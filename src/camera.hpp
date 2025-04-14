#pragma once
#include "vec3.hpp"
#include "ray.hpp"

struct Camera {
    Vec3 position;
    Vec3 forward;
    Vec3 up;
    float fov;

    Camera() {}
    Camera(Vec3 pos, Vec3 fwd, Vec3 up, float fov)
        : position(pos), forward(fwd.normalize()), up(up.normalize()), fov(fov) {}

    Ray getRay(float u, float v) const {
        Vec3 right = Vec3(
            up.y * forward.z - up.z * forward.y,
            up.z * forward.x - up.x * forward.z,
            up.x * forward.y - up.y * forward.x
        );
        float aspect = 800.0f / 600.0f; // hardcoded for now
        float scale = std::tan(fov * 0.5 * M_PI / 180.0f);

        Vec3 rayDir = (forward + right * ((2 * u - 1) * aspect * scale) + up * ((2 * v - 1) * scale)).normalize();
        return Ray(position, rayDir);
    }
};