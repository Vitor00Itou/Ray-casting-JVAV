#pragma once
#include "vec3.hpp"
#include "object.hpp"

struct LightPoint : public Object {
	Vec3 position;
	Color lightColor;

	LightPoint(Vec3 position, Color lightColor) : position(position), lightColor(lightColor) {}
	bool isEmitter() const { return true; }
	Color getIntensity() const { return lightColor; }

	HitInfo intersect(const Ray& ray) const {
        return HitInfo{0, Vec3(), Vec3(), SurfaceCoord{0, 0}, false};
    }

	Vec3 getLightDir(const HitInfo& hit){
		return (position - hit.point).normalize();
	}
};