#pragma once
#include "vec3.hpp"
#include "ray.hpp"
#include <optional>

struct HitInfo {
    float t;
    Vec3 point;
    Vec3 normal;
};

struct Sphere {
    Vec3 center;
    float radius;

    Sphere(Vec3 c, float r) : center(c), radius(r) {}

    std::optional<HitInfo> intersect(const Ray& ray) const {
        Vec3 oc = ray.origin - center;
        float a = ray.direction.dot(ray.direction);
        float b = 2.0f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) return std::nullopt;
        float t = (-b - std::sqrt(discriminant)) / (2.0f * a);
        if (t < 0) return std::nullopt;

        Vec3 point = ray.origin + ray.direction * t;
        Vec3 normal = (point - center).normalize();
        return HitInfo{t, point, normal};
    }
};