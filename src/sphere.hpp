#pragma once

#include <optional>

#include "hitinfo.hpp"
#include "ray.hpp"
#include "texture.hpp"
#include <cmath>

struct Sphere {
    Vec3 center;
    float radius;
    Color color;
    Texture texture;

    Sphere(Vec3 c, float r) : center(c), radius(r) { }
    Sphere(Vec3 c, float r, Color color) : center(c), radius(r), color(color) {
        texture = Texture(color, 1, 1);
    }
    Sphere(Vec3 c, float r, const char* textureName) : center(c), radius(r) {
        texture = Texture(textureName);
    }

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

        // Coordenadas normalizdas NA SUPERFICIE do objeto
        float u = 0.5 + atan2(normal.z, normal.x) / (2 * M_PI);
        float v = 0.5 - asin(normal.y) / M_PI;
        return HitInfo{t, point, normal, SurfaceCoord{u, v}, true};
    }
};