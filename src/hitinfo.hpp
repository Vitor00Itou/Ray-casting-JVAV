#pragma once

#include "vec3.hpp"

/**
 * @brief 2D Surface UV coordinates for texture mapping.
 */
struct SurfaceCoord {
    float u = 0.0f;
    float v = 0.0f;
};

/**
 * @brief Structure containing ray-object intersection query details.
 */
struct HitInfo {
    float t = 0.0f;               ///< Ray parameter distance to hit point
    Vec3 point;                   ///< World-space 3D hit point
    Vec3 normal;                  ///< Surface normal vector at hit point
    SurfaceCoord surfaceCoord;    ///< UV texture coordinates
    bool hasHit = false;          ///< True if an intersection occurred
};
