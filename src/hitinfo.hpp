#pragma once

#include "vec3.hpp"

struct SurfaceCoord {
	float u, v;
};

struct HitInfo {
    float t;
    Vec3 point;
    Vec3 normal;
	SurfaceCoord surfaceCoord;
    bool hasHit;
};