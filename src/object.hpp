#pragma once

#include "hitinfo.hpp"
#include "color.hpp"
#include "ray.hpp"
#include "texture.hpp"

enum ObjectType {
    UNDEFINED,
    SPHERE,
    PLANE,
    BOX,
    LIGHT_POINT
};

struct Object {
    ObjectType type = UNDEFINED;
    virtual HitInfo intersect(const Ray& ray) const = 0;
    virtual Vec3 getCenter() const = 0;
    virtual ~Object() {}

    virtual float getSpecularShininess() const {
        return 32;
    }
    
    virtual Color getColor(const HitInfo& hit) const { return Color(0, 0, 0); };

    virtual bool isEmitter() const { return false; }
    virtual Color getIntensity() const { return Color(0, 0, 0); }
    virtual Vec3 getLightDir(const HitInfo& hit) { return Vec3(0, 0, 0); }
    
    virtual bool isReflective() const { return false; }
    virtual bool isTransparent() const { return false; }

    virtual float getTransparency() const { return 0; }
    virtual float getRefractiveIndex() const { return 1; }
    virtual float getReflectionCoefficient() const { return 0; }
    
    virtual bool isInert() const { return false; }
};  