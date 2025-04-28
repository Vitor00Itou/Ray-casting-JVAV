#pragma once

#include "object.hpp"
#include "hitinfo.hpp"
#include "ray.hpp"
#include "texture.hpp"
#include <algorithm> // std::min, std::max

struct Box : public Object {
    Vec3 minCorner;
    Vec3 maxCorner;
    Color color;
    Texture texture;
    bool _isEmitter = false;
    bool _isMirror = false;
    float transparency = 0.0f;      
    float refractiveIndex = 1.0f;    

    Box(Vec3 minC, Vec3 maxC) : minCorner(minC), maxCorner(maxC) {
        this->type = BOX;
        minCorner = Vec3(std::min(minC.x, maxC.x), std::min(minC.y, maxC.y), std::min(minC.z, maxC.z));
        maxCorner = Vec3(std::max(minC.x, maxC.x), std::max(minC.y, maxC.y), std::max(minC.z, maxC.z));

        texture = Texture(Color(1.0f, 1.0f, 1.0f));
    }

    Box(Vec3 minC, Vec3 maxC, bool isMirror) : minCorner(minC), maxCorner(maxC), _isMirror(isMirror) {
        this->type = BOX;
        minCorner = Vec3(std::min(minC.x, maxC.x), std::min(minC.y, maxC.y), std::min(minC.z, maxC.z));
        maxCorner = Vec3(std::max(minC.x, maxC.x), std::max(minC.y, maxC.y), std::max(minC.z, maxC.z));

        texture = Texture(Color(1.0f, 1.0f, 1.0f));
    }

    Box(Vec3 minC, Vec3 maxC, Color color) : minCorner(minC), maxCorner(maxC), color(color) {
        this->type = BOX;
        minCorner = Vec3(std::min(minC.x, maxC.x), std::min(minC.y, maxC.y), std::min(minC.z, maxC.z));
        maxCorner = Vec3(std::max(minC.x, maxC.x), std::max(minC.y, maxC.y), std::max(minC.z, maxC.z));

        texture = Texture(color);
    }

    Box(Vec3 minC, Vec3 maxC, Color color, bool isEmitter) : minCorner(minC), maxCorner(maxC), color(color), _isEmitter(isEmitter) {
        this->type = BOX;
        minCorner = Vec3(std::min(minC.x, maxC.x), std::min(minC.y, maxC.y), std::min(minC.z, maxC.z));
        maxCorner = Vec3(std::max(minC.x, maxC.x), std::max(minC.y, maxC.y), std::max(minC.z, maxC.z));

        texture = Texture(color);
    }

    Box(Vec3 minC, Vec3 maxC, const char* textureName) : minCorner(minC), maxCorner(maxC) {
        this->type = BOX;
        minCorner = Vec3(std::min(minC.x, maxC.x), std::min(minC.y, maxC.y), std::min(minC.z, maxC.z));
        maxCorner = Vec3(std::max(minC.x, maxC.x), std::max(minC.y, maxC.y), std::max(minC.z, maxC.z));

        texture = Texture(textureName);
    }

    Box(Vec3 minC, Vec3 maxC, Color color, const char* textureName, bool isEmitter) : minCorner(minC), maxCorner(maxC), color(color), _isEmitter(isEmitter) {
        this->type = BOX;
        minCorner = Vec3(std::min(minC.x, maxC.x), std::min(minC.y, maxC.y), std::min(minC.z, maxC.z));
        maxCorner = Vec3(std::max(minC.x, maxC.x), std::max(minC.y, maxC.y), std::max(minC.z, maxC.z));

        texture = Texture(textureName);
    }

    Color getColor(const HitInfo& hit) const override {
        return texture.getColorFromImgCoordinates(hit.surfaceCoord);
    }

    Vec3 getCenter() const override{
        return (minCorner + maxCorner)/2;
    } 

    bool isMirror() const override{
        return _isMirror;
    }

    bool isTransparent() const override { 
        return transparency > 0.0f; 
    }
    float getTransparency() const override { 
        return transparency; 
    }
    float getRefractiveIndex() const override { 
        return refractiveIndex; 
    }


    HitInfo intersect(const Ray& ray) const override {
        // Método do Slab para AABB (axis aligned bounding box)
        float tmin = (minCorner.x - ray.origin.x) / ray.direction.x;
        float tmax = (maxCorner.x - ray.origin.x) / ray.direction.x;
        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (minCorner.y - ray.origin.y) / ray.direction.y;
        float tymax = (maxCorner.y - ray.origin.y) / ray.direction.y;
        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return HitInfo{0, Vec3(), Vec3(), false};

        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (minCorner.z - ray.origin.z) / ray.direction.z;
        float tzmax = (maxCorner.z - ray.origin.z) / ray.direction.z;
        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return HitInfo{0, Vec3(), Vec3(), false};

        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;

        if (tmin < 0) return HitInfo{0, Vec3(), Vec3(), false}; // atrás da câmera

        Vec3 hitPoint = ray.origin + ray.direction * tmin;

        // Descobrir a normal do lado atingido
        Vec3 normal(0, 0, 0);
        constexpr float epsilon = 1e-4;
        if (std::abs(hitPoint.x - minCorner.x) < epsilon) normal = Vec3(-1, 0, 0);
        else if (std::abs(hitPoint.x - maxCorner.x) < epsilon) normal = Vec3(1, 0, 0);
        else if (std::abs(hitPoint.y - minCorner.y) < epsilon) normal = Vec3(0, -1, 0);
        else if (std::abs(hitPoint.y - maxCorner.y) < epsilon) normal = Vec3(0, 1, 0);
        else if (std::abs(hitPoint.z - minCorner.z) < epsilon) normal = Vec3(0, 0, -1);
        else if (std::abs(hitPoint.z - maxCorner.z) < epsilon) normal = Vec3(0, 0, 1);

        // Coordenadas de textura: simples projeção nas faces
        float u = 0.0f, v = 0.0f;
        if (normal.x != 0) { // faces paralelas ao plano yz
            u = (hitPoint.z - minCorner.z) / (maxCorner.z - minCorner.z);
            v = (hitPoint.y - minCorner.y) / (maxCorner.y - minCorner.y);
        } else if (normal.y != 0) { // faces paralelas ao plano xz
            u = (hitPoint.x - minCorner.x) / (maxCorner.x - minCorner.x);
            v = (hitPoint.z - minCorner.z) / (maxCorner.z - minCorner.z);
        } else if (normal.z != 0) { // faces paralelas ao plano xy
            u = (hitPoint.x - minCorner.x) / (maxCorner.x - minCorner.x);
            v = (hitPoint.y - minCorner.y) / (maxCorner.y - minCorner.y);
        }

        return HitInfo{tmin, hitPoint, normal, SurfaceCoord{u, v}, true};
    }

    bool isEmitter() const override {
        return _isEmitter;
    }

    Vec3 getLightDir(const HitInfo& hit) override {
        Vec3 center = (minCorner + maxCorner) * 0.5f;
        return (center - hit.point).normalize();
    }

    Color getIntensity() const override {
        return color;
    }
};
