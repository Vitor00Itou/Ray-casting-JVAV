#pragma once

#include <cmath>
#include "object.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "hitinfo.hpp"
#include "texture.hpp"

struct Plane : public Object{
    Vec3 point;      // Um ponto qualquer no plano
    Vec3 normal;     // A normal do plano (normalizada)
    Color color;     // Cor base
    Texture texture; // Textura
    float specularShininess = 32;
    float reflectionCoefficient = 0.0f;
    float transparency = 0.0f;      // 0 = opaco, 1 = totalmente transparente
    float refractiveIndex = 1.0f;    // 1.0 = ar, 1.5 = vidro, 2.4 = diamante
    bool _isInert = false;

    Color getColor(const HitInfo& hit) const override {
        return texture.getColorFromImgCoordinates(hit.surfaceCoord);
    }

    Plane(Vec3 p, Vec3 n) : point(p), normal(n.normalize()) {
        this->type = PLANE;
        texture = Texture(Color(1.0f, 1.0f, 1.0f));
    }

    Plane(Vec3 p, Vec3 n, float reflectionCoefficient) : point(p), normal(n.normalize()),reflectionCoefficient(reflectionCoefficient){
        this->type = PLANE;
        texture = Texture(Color(1.0f, 1.0f, 1.0f));
    }


    Plane(Vec3 p, Vec3 n, Color color) : point(p), normal(n.normalize()), color(color) {
        this->type = PLANE;
        texture = Texture(color);
    }

    Plane(Vec3 p, Vec3 n, const char* textureName) : point(p), normal(n.normalize()) {
        this->type = PLANE;
        texture = Texture(textureName);
    }

    Plane(Vec3 p, Vec3 n, Color color, const char* textureName, float specularShininess, float reflectionCoefficient, float transparency, float refractiveIndex, bool isInert) : point(p), normal(n.normalize()), color(color), specularShininess(specularShininess), reflectionCoefficient(reflectionCoefficient), transparency(transparency), refractiveIndex(refractiveIndex), _isInert(isInert) {
        this->type = PLANE;
        texture = Texture(textureName, color);
    }

    Vec3 getCenter() const override{
        return point;
    };

    bool isReflective() const override{
        return reflectionCoefficient > 0.0f;
    }

    float getSpecularShininess() const override {
        return specularShininess;
    }

    float getReflectionCoefficient() const override{
        return reflectionCoefficient;
    }

    virtual bool isInert() const override { return _isInert; }


    HitInfo intersect(const Ray& ray) const {
        float denom = normal.dot(ray.direction);
        if (std::abs(denom) > 1e-6) {
            float t = (point - ray.origin).dot(normal) / denom;
            if (t >= 0 ) {
                Vec3 hitPoint = ray.origin + ray.direction * t;
                Vec3 adjustedNormal = normal;
    
                // Ajusta a normal se o raio estiver "por trás" do plano
                if (ray.direction.dot(normal) > 0) {
                    adjustedNormal = normal * (-1);
                }
    
                // Coordenadas UV fictícias (úteis para texturas)
                float u = hitPoint.x - std::floor(hitPoint.x);
                float v = hitPoint.z - std::floor(hitPoint.z);
    
                return HitInfo{t, hitPoint, adjustedNormal, SurfaceCoord{u, v}, true};
            }
        }
        return HitInfo{0, Vec3(), Vec3(), SurfaceCoord{0, 0}, false};
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
