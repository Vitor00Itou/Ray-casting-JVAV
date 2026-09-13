#pragma once

#include "hitinfo.hpp"
#include "color.hpp"
#include "ray.hpp"

enum ObjectType {
    UNDEFINED,
    SPHERE,
    PLANE,
    BOX,
    LIGHT_POINT
};

/**
 * @brief Abstract Base Class representing a scene object or light source.
 */
struct Object {
    ObjectType type = UNDEFINED;

    virtual ~Object() = default;

    // Ray intersection and center queries
    virtual HitInfo intersect(const Ray& ray) const = 0;
    virtual Vec3 getCenter() const = 0;

    // Material properties
    virtual float getSpecularShininess() const { return 32.0f; }
    virtual Color getColor(const HitInfo& hit) const { return Color(0, 0, 0); }

    // Emission & Light properties
    virtual bool isEmitter() const { return false; }
    virtual Color getIntensity() const { return Color(0, 0, 0); }
    virtual Vec3 getLightDir(const HitInfo& hit) const { return Vec3(0, 0, 0); }

    // Reflection & Refraction properties
    virtual bool isReflective() const { return false; }
    virtual bool isTransparent() const { return false; }
    virtual float getTransparency() const { return 0.0f; }
    virtual float getRefractiveIndex() const { return 1.0f; }
    virtual float getReflectionCoefficient() const { return 0.0f; }

    // Render mode flag (skip lighting computation if inert)
    virtual bool isInert() const { return false; }

    // Dynamic Object Manipulation
    virtual void move(const Vec3& delta) {}
    virtual void addLuminosity(const Color& delta) {}
};  
