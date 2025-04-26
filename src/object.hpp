#pragma once

#include "hitinfo.hpp"
#include "color.hpp"
#include "ray.hpp"
#include "texture.hpp"

struct Object {
    virtual HitInfo intersect(const Ray& ray) const = 0;
    virtual bool isEmitter() const { return false; }
    virtual Color getEmission() const { return Color(0, 0, 0); }
    virtual ~Object() {}

    virtual Color getColor(const HitInfo& hit) const = 0;
};