#pragma once

#include <cmath>
#include "object.hpp"
#include "hitinfo.hpp"
#include "ray.hpp"
#include "texture.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Sphere primitive supporting texturing, reflections, transparency, and emission.
 */
struct Sphere : public Object {
    Vec3 center;
    float radius;
    Color color;
    Texture texture;
    bool _isEmitter = false;
    float specularShininess = 32.0f;
    float reflectionCoefficient = 0.0f;
    float transparency = 0.0f;       // 0.0 = opaque, 1.0 = fully transparent
    float refractiveIndex = 1.0f;    // 1.0 = air, 1.5 = glass, 2.4 = diamond
    bool _isInert = false;

    Sphere(Vec3 c, float r) : center(c), radius(r), color(1.0f, 1.0f, 1.0f), texture(Color(1.0f, 1.0f, 1.0f)) {
        this->type = SPHERE;
    }

    Sphere(Vec3 c, float r, float refl) : center(c), radius(r), color(1.0f, 1.0f, 1.0f), texture(Color(1.0f, 1.0f, 1.0f)), reflectionCoefficient(refl) {
        this->type = SPHERE;
    }

    Sphere(Vec3 c, float r, Color col) : center(c), radius(r), color(col), texture(col) {
        this->type = SPHERE;
    }

    Sphere(Vec3 c, float r, Color col, bool isEmitter) : center(c), radius(r), color(col), texture(col), _isEmitter(isEmitter) {
        this->type = SPHERE;
    }

    Sphere(Vec3 c, float r, const char* textureName) : center(c), radius(r), color(1.0f, 1.0f, 1.0f), texture(textureName, Color(1.0f, 1.0f, 1.0f)) {
        this->type = SPHERE;
    }

    Sphere(Vec3 c, float r, const char* textureName, bool isEmitter) : center(c), radius(r), color(1.0f, 1.0f, 1.0f), texture(textureName, Color(1.0f, 1.0f, 1.0f)), _isEmitter(isEmitter) {
        this->type = SPHERE;
    }

    Sphere(Vec3 c, float r, Color col, const char* textureName, bool isEmitter) : center(c), radius(r), color(col), texture(textureName, col), _isEmitter(isEmitter) {
        this->type = SPHERE;
    }

    Sphere(Vec3 c, float r, Color col, const char* textureName, bool isEmitter, float specShininess, float refl, float transp, float refrIndex, bool isInert)
        : center(c), radius(r), color(col), texture(textureName, col), _isEmitter(isEmitter), specularShininess(specShininess), reflectionCoefficient(refl), transparency(transp), refractiveIndex(refrIndex), _isInert(isInert) {
        this->type = SPHERE;
    }

    Color getColor(const HitInfo& hit) const override {
        return texture.getColorFromImgCoordinates(hit.surfaceCoord);
    }

    Vec3 getCenter() const override { return center; }
    float getSpecularShininess() const override { return specularShininess; }
    bool isReflective() const override { return reflectionCoefficient > 0.0f; }
    bool isTransparent() const override { return transparency > 0.0f; }
    float getTransparency() const override { return transparency; }
    float getRefractiveIndex() const override { return refractiveIndex; }
    float getReflectionCoefficient() const override { return reflectionCoefficient; }
    bool isEmitter() const override { return _isEmitter; }
    Color getIntensity() const override { return color; }
    bool isInert() const override { return _isInert; }

    Vec3 getLightDir(const HitInfo& hit) const override {
        return (center - hit.point).normalize();
    }

    void move(const Vec3& delta) override {
        center = center + delta;
    }

    void addLuminosity(const Color& delta) override {
        if (_isEmitter) {
            color = color + delta;
        }
    }

    /**
     * @brief Ray-Sphere intersection test using geometric quadratic equation.
     */
    HitInfo intersect(const Ray& ray) const override {
        Vec3 oc = ray.origin - center;
        float a = ray.direction.dot(ray.direction);
        float b = 2.0f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f) return HitInfo{0, Vec3(), Vec3(), SurfaceCoord{0, 0}, false};

        float sqrtD = std::sqrt(discriminant);
        float t = (-b - sqrtD) / (2.0f * a);
        if (t < 0.001f) {
            t = (-b + sqrtD) / (2.0f * a);
            if (t < 0.001f) return HitInfo{0, Vec3(), Vec3(), SurfaceCoord{0, 0}, false};
        }

        Vec3 point = ray.origin + ray.direction * t;
        Vec3 normal = (point - center).normalize();

        // Spherical mapping for UV surface coordinates
        float u = 0.5f + std::atan2(normal.z, normal.x) / (2.0f * static_cast<float>(M_PI));
        float v = 0.5f - std::asin(std::clamp(normal.y, -1.0f, 1.0f)) / static_cast<float>(M_PI);

        return HitInfo{t, point, normal, SurfaceCoord{u, v}, true};
    }
};

