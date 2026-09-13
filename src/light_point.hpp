#pragma once

#include "vec3.hpp"
#include "object.hpp"

/**
 * @brief Point light source emitting uniform color intensity in all directions.
 */
struct LightPoint : public Object {
    Vec3 position;
    Color lightColor;

    LightPoint(const Vec3& pos, const Color& col) : position(pos), lightColor(col) {
        this->type = LIGHT_POINT;
    }

    bool isEmitter() const override { return true; }
    Color getIntensity() const override { return lightColor; }

    HitInfo intersect(const Ray& ray) const override {
        return HitInfo{0.0f, Vec3(), Vec3(), SurfaceCoord{0, 0}, false};
    }

    Vec3 getLightDir(const HitInfo& hit) const override {
        return (position - hit.point).normalize();
    }

    Vec3 getCenter() const override {
        return position;
    }

    void move(const Vec3& delta) override {
        position = position + delta;
    }

    void addLuminosity(const Color& delta) override {
        lightColor = lightColor + delta;
    }
};
