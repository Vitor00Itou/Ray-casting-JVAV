#pragma once
#include "vec3.hpp"
#include "ray.hpp"
#include <optional>

struct HitInfo {
    float t;
    Vec3 point;
    Vec3 normal;
    bool hasHit;
};

struct Color {
    float r, g, b;

    Color() : r(1.0), g(1.0), b(1.0) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
};

struct Sphere {
    Vec3 center;
    float radius;
    Color color;

    Sphere(Vec3 c, float r) : center(c), radius(r) {
        color = Color(1.0, 1.0, 1.0);
    }
    Sphere(Vec3 c, float r, Color color) : center(c), radius(r), color(color) {}

    HitInfo intersect(const Ray& ray) const {
        Vec3 oc = ray.origin - center;
        float a = ray.direction.dot(ray.direction);
        float b = 2.0f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) return HitInfo{0, Vec3(), Vec3(), false};
        float t = (-b - std::sqrt(discriminant)) / (2.0f * a);
        if (t < 0) return HitInfo{0, Vec3(), Vec3(), false};

        Vec3 point = ray.origin + ray.direction * t;
        Vec3 normal = (point - center).normalize();
        return HitInfo{t, point, normal, true};
    }
};