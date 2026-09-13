#pragma once

#include "scene.hpp"
#include "camera.hpp"
#include <vector>
#include <limits>
#include <algorithm>

/**
 * @brief Fast preview renderer displaying object base colors without ray tracing lighting.
 */
struct RayObjectRenderer {
    int width;
    int height;
    std::vector<unsigned char> framebuffer;

    RayObjectRenderer(int w, int h) : width(w), height(h), framebuffer(w * h * 3, 0) {}

    void render(const Scene& scene, const Camera& camera) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float u = (x + 0.5f) / static_cast<float>(width);
                float v = (y + 0.5f) / static_cast<float>(height);
                Ray ray = camera.getRay(u, v);

                Color finalColor(0.2f, 0.2f, 0.4f); // Background color
                float closestT = std::numeric_limits<float>::max();

                for (const auto& obj : scene.objects) {
                    if (!obj) continue;
                    HitInfo hit = obj->intersect(ray);
                    if (hit.hasHit && hit.t < closestT) {
                        closestT = hit.t;
                        finalColor = obj->getColor(hit);
                    }
                }

                int i = (y * width + x) * 3;
                framebuffer[i]     = static_cast<unsigned char>(std::clamp(finalColor.r, 0.0f, 1.0f) * 255.0f);
                framebuffer[i + 1] = static_cast<unsigned char>(std::clamp(finalColor.g, 0.0f, 1.0f) * 255.0f);
                framebuffer[i + 2] = static_cast<unsigned char>(std::clamp(finalColor.b, 0.0f, 1.0f) * 255.0f);
            }
        }
    }

    const std::vector<unsigned char>& getFramebuffer() const {
        return framebuffer;
    }
};

