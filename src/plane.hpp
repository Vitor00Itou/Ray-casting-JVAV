#pragma once

#include <cmath>
#include "object.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "hitinfo.hpp"
#include "texture.hpp"

/**
 * @brief Infinite plane primitive supporting texturing, surface normals, reflection, and refraction.
 */
struct Plane : public Object {
    Vec3 point;               ///< Point on the plane
    Vec3 normal;              ///< Normalized plane surface normal
    Color color;              ///< Base color
    Texture texture;          ///< Texture mapping
    float specularShininess = 32.0f;
    float reflectionCoefficient = 0.0f;
    float transparency = 0.0f;
    float refractiveIndex = 1.0f;
    bool _isInert = false;

    Plane(Vec3 p, Vec3 n) : point(p), normal(n.normalize()), color(1.0f, 1.0f, 1.0f), texture(Color(1.0f, 1.0f, 1.0f)) {
        this->type = PLANE;
    }

    Plane(Vec3 p, Vec3 n, float refl) : point(p), normal(n.normalize()), color(1.0f, 1.0f, 1.0f), texture(Color(1.0f, 1.0f, 1.0f)), reflectionCoefficient(refl) {
        this->type = PLANE;
    }

    Plane(Vec3 p, Vec3 n, Color col) : point(p), normal(n.normalize()), color(col), texture(col) {
        this->type = PLANE;
    }

    Plane(Vec3 p, Vec3 n, const char* textureName) : point(p), normal(n.normalize()), color(1.0f, 1.0f, 1.0f), texture(textureName, Color(1.0f, 1.0f, 1.0f)) {
        this->type = PLANE;
    }

    Plane(Vec3 p, Vec3 n, Color col, const char* textureName, float specShininess, float refl, float transp, float refrIndex, bool isInert)
        : point(p), normal(n.normalize()), color(col), texture(textureName, col), specularShininess(specShininess), reflectionCoefficient(refl), transparency(transp), refractiveIndex(refrIndex), _isInert(isInert) {
        this->type = PLANE;
    }

    Color getColor(const HitInfo& hit) const override {
        return texture.getColorFromImgCoordinates(hit.surfaceCoord);
    }

    Vec3 getCenter() const override { return point; }
    bool isReflective() const override { return reflectionCoefficient > 0.0f; }
    float getSpecularShininess() const override { return specularShininess; }
    float getReflectionCoefficient() const override { return reflectionCoefficient; }
    bool isTransparent() const override { return transparency > 0.0f; }
    float getTransparency() const override { return transparency; }
    float getRefractiveIndex() const override { return refractiveIndex; }
    bool isInert() const override { return _isInert; }

    void move(const Vec3& delta) override {
        point = point + delta;
    }

    void addLuminosity(const Color& delta) override {
        // Planes can act as emitters if configured
        color = color + delta;
    }

    /**
     * @brief Ray-Plane analytical intersection calculation.
     */
    HitInfo intersect(const Ray& ray) const override {
        float denom = normal.dot(ray.direction);
        if (std::abs(denom) > 1e-6f) {
            float t = (point - ray.origin).dot(normal) / denom;
            if (t >= 0.001f) {
                Vec3 hitPoint = ray.origin + ray.direction * t;
                Vec3 adjustedNormal = normal;

                // Adjust normal if ray hits plane from behind
                if (ray.direction.dot(normal) > 0.0f) {
                    adjustedNormal = normal * (-1.0f);
                }

                // Planar UV coordinates repetition based on floor mapping
                float u = hitPoint.x - std::floor(hitPoint.x);
                float v = hitPoint.z - std::floor(hitPoint.z);

                return HitInfo{t, hitPoint, adjustedNormal, SurfaceCoord{u, v}, true};
            }
        }
        return HitInfo{0.0f, Vec3(), Vec3(), SurfaceCoord{0, 0}, false};
    }
};

