#pragma once

#include <optional>

#include "object.hpp"
#include "hitinfo.hpp"
#include "ray.hpp"
#include "texture.hpp"
#include <cmath>

struct Sphere : public Object {
    Vec3 center;
    float radius;
    Color color;
    Texture texture;
    bool _isEmitter = false;
    float specularShininess = 32;
    bool _isMirror = false;
    float transparency = 0.0f;      // 0 = opaco, 1 = totalmente transparente
    float refractiveIndex = 1.0f;    // 1.0 = ar, 1.5 = vidro, 2.4 = diamante

    Color getColor(const HitInfo& hit) const override {
        return texture.getColorFromImgCoordinates(hit.surfaceCoord);
    }

    Vec3 getCenter() const override {
        return center;
    };

    float getSpecularShininess() const override {
        return specularShininess;
    }

    bool isMirror() const override{
        return _isMirror;
    }


    Sphere(Vec3 c, float r) : center(c), radius(r) { 
        this->type = SPHERE;
        texture = Texture(Color(1.0f, 1.0f, 1.0f));
    }

    Sphere(Vec3 c, float r, bool isMirror) : center(c), radius(r), _isMirror(isMirror){ 
        this->type = SPHERE;
        texture = Texture(Color(1.0f, 1.0f, 1.0f));
    }

    Sphere(Vec3 c, float r, Color color) : center(c), radius(r), color(color) {
        this->type = SPHERE;
        texture = Texture(color);
    }
    Sphere(Vec3 c, float r, Color color, bool isEmitter) : center(c), radius(r), color(color), _isEmitter(isEmitter) {
        this->type = SPHERE;
        texture = Texture(color);
    }
    Sphere(Vec3 c, float r, const char* textureName) : center(c), radius(r) {
        this->type = SPHERE;
        texture = Texture(textureName);
    }
    Sphere(Vec3 c, float r, const char* textureName, bool isEmitter) : center(c), radius(r), _isEmitter(isEmitter) {
        this->type = SPHERE;
        texture = Texture(textureName);
    }
    Sphere(Vec3 c, float r, Color color, const char* textureName, bool isEmitter) : center(c), radius(r), color(color), _isEmitter(isEmitter) {
        this->type = SPHERE;
        texture = Texture(textureName);
    }
    Sphere(Vec3 c, float r, Color color, const char* textureName, bool isEmitter, float specularShininess) : center(c), radius(r), color(color), _isEmitter(isEmitter), specularShininess(specularShininess) {
        this->type = SPHERE;
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

    bool isEmitter() const override {
        return _isEmitter;
    }

    Vec3 getLightDir(const HitInfo& hit){
		return (center - hit.point).normalize();
	}

    Color getIntensity() const override {
        return color;
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
};
