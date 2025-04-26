#pragma once

#include "hitinfo.hpp"
#include "color.hpp"
#include "ray.hpp"
#include "texture.hpp"

struct Object {
    virtual HitInfo intersect(const Ray& ray) const = 0;
    virtual ~Object() {}
    
    virtual Color getColor(const HitInfo& hit) const { return Color(0, 0, 0); };

    virtual bool isEmitter() const { return false; }
    virtual Color getIntensity() const { return Color(0, 0, 0); }
    virtual Vec3 getLightDir(const HitInfo& hit) { return Vec3(0, 0, 0); }
};