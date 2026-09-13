#pragma once

#include "object.hpp"
#include "hitinfo.hpp"
#include "ray.hpp"
#include "texture.hpp"
#include <algorithm>

/**
 * @brief Axis-Aligned Bounding Box (AABB) primitive supporting texturing, slab intersection, reflection, and refraction.
 */
struct Box : public Object {
    Vec3 minCorner;
    Vec3 maxCorner;
    Color color;
    Texture texture;
    bool _isEmitter = false;
    float specularShininess = 32.0f;
    float reflectionCoefficient = 0.0f;
    float transparency = 0.0f;
    float refractiveIndex = 1.0f;
    bool _isInert = false;

    void normalizeBounds(Vec3 minC, Vec3 maxC) {
        minCorner = Vec3(std::min(minC.x, maxC.x), std::min(minC.y, maxC.y), std::min(minC.z, maxC.z));
        maxCorner = Vec3(std::max(minC.x, maxC.x), std::max(minC.y, maxC.y), std::max(minC.z, maxC.z));
    }

    Box(Vec3 minC, Vec3 maxC) : color(1.0f, 1.0f, 1.0f), texture(Color(1.0f, 1.0f, 1.0f)) {
        this->type = BOX;
        normalizeBounds(minC, maxC);
    }

    Box(Vec3 minC, Vec3 maxC, float refl) : color(1.0f, 1.0f, 1.0f), texture(Color(1.0f, 1.0f, 1.0f)), reflectionCoefficient(refl) {
        this->type = BOX;
        normalizeBounds(minC, maxC);
    }

    Box(Vec3 minC, Vec3 maxC, Color col) : color(col), texture(col) {
        this->type = BOX;
        normalizeBounds(minC, maxC);
    }

    Box(Vec3 minC, Vec3 maxC, Color col, bool isEmitter) : color(col), texture(col), _isEmitter(isEmitter) {
        this->type = BOX;
        normalizeBounds(minC, maxC);
    }

    Box(Vec3 minC, Vec3 maxC, const char* textureName) : color(1.0f, 1.0f, 1.0f), texture(textureName, Color(1.0f, 1.0f, 1.0f)) {
        this->type = BOX;
        normalizeBounds(minC, maxC);
    }

    Box(Vec3 minC, Vec3 maxC, Color col, const char* textureName, bool isEmitter) : color(col), texture(textureName, col), _isEmitter(isEmitter) {
        this->type = BOX;
        normalizeBounds(minC, maxC);
    }

    Box(Vec3 minC, Vec3 maxC, Color col, const char* textureName, bool isEmitter, float specShininess, float refl, float transp, float refrIndex, bool isInert)
        : color(col), texture(textureName, col), _isEmitter(isEmitter), specularShininess(specShininess), reflectionCoefficient(refl), transparency(transp), refractiveIndex(refrIndex), _isInert(isInert) {
        this->type = BOX;
        normalizeBounds(minC, maxC);
    }

    Color getColor(const HitInfo& hit) const override {
        return texture.getColorFromImgCoordinates(hit.surfaceCoord);
    }

    Vec3 getCenter() const override {
        return (minCorner + maxCorner) * 0.5f;
    }

    bool isReflective() const override { return reflectionCoefficient > 0.0f; }
    bool isTransparent() const override { return transparency > 0.0f; }
    float getTransparency() const override { return transparency; }
    float getRefractiveIndex() const override { return refractiveIndex; }
    float getReflectionCoefficient() const override { return reflectionCoefficient; }
    bool isEmitter() const override { return _isEmitter; }
    Color getIntensity() const override { return color; }
    bool isInert() const override { return _isInert; }

    Vec3 getLightDir(const HitInfo& hit) const override {
        return (getCenter() - hit.point).normalize();
    }

    void move(const Vec3& delta) override {
        minCorner = minCorner + delta;
        maxCorner = maxCorner + delta;
    }

    void addLuminosity(const Color& delta) override {
        if (_isEmitter) {
            color = color + delta;
        }
    }

    /**
     * @brief Ray-Box intersection calculation using the Slab method for AABB.
     */
    HitInfo intersect(const Ray& ray) const override {
        float tmin = (minCorner.x - ray.origin.x) / ray.direction.x;
        float tmax = (maxCorner.x - ray.origin.x) / ray.direction.x;
        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (minCorner.y - ray.origin.y) / ray.direction.y;
        float tymax = (maxCorner.y - ray.origin.y) / ray.direction.y;
        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return HitInfo{0.0f, Vec3(), Vec3(), SurfaceCoord{0, 0}, false};

        if (tymin > tmin) tmin = tymin;
        if (tymax < tmax) tmax = tymax;

        float tzmin = (minCorner.z - ray.origin.z) / ray.direction.z;
        float tzmax = (maxCorner.z - ray.origin.z) / ray.direction.z;
        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return HitInfo{0.0f, Vec3(), Vec3(), SurfaceCoord{0, 0}, false};

        if (tzmin > tmin) tmin = tzmin;
        if (tzmax < tmax) tmax = tzmax;

        if (tmin < 0.001f) return HitInfo{0.0f, Vec3(), Vec3(), SurfaceCoord{0, 0}, false};

        Vec3 hitPoint = ray.origin + ray.direction * tmin;

        // Compute hit face surface normal
        Vec3 normal(0, 0, 0);
        constexpr float epsilon = 1e-4f;
        if (std::abs(hitPoint.x - minCorner.x) < epsilon) normal = Vec3(-1, 0, 0);
        else if (std::abs(hitPoint.x - maxCorner.x) < epsilon) normal = Vec3(1, 0, 0);
        else if (std::abs(hitPoint.y - minCorner.y) < epsilon) normal = Vec3(0, -1, 0);
        else if (std::abs(hitPoint.y - maxCorner.y) < epsilon) normal = Vec3(0, 1, 0);
        else if (std::abs(hitPoint.z - minCorner.z) < epsilon) normal = Vec3(0, 0, -1);
        else if (std::abs(hitPoint.z - maxCorner.z) < epsilon) normal = Vec3(0, 0, 1);

        // UV projection mapping per box face
        float u = 0.0f, v = 0.0f;
        float dx = maxCorner.x - minCorner.x;
        float dy = maxCorner.y - minCorner.y;
        float dz = maxCorner.z - minCorner.z;

        if (normal.x != 0.0f && dz > 0 && dy > 0) {
            u = (hitPoint.z - minCorner.z) / dz;
            v = (hitPoint.y - minCorner.y) / dy;
        } else if (normal.y != 0.0f && dx > 0 && dz > 0) {
            u = (hitPoint.x - minCorner.x) / dx;
            v = (hitPoint.z - minCorner.z) / dz;
        } else if (normal.z != 0.0f && dx > 0 && dy > 0) {
            u = (hitPoint.x - minCorner.x) / dx;
            v = (hitPoint.y - minCorner.y) / dy;
        }

        return HitInfo{tmin, hitPoint, normal, SurfaceCoord{u, v}, true};
    }
};

