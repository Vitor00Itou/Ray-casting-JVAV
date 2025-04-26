#pragma once
#include "scene.hpp"
#include "camera.hpp"
#include <iostream>
#include <vector>

struct RayObjectRenderer {
    int width, height;
    std::vector<unsigned char> framebuffer;

    RayObjectRenderer(int w, int h) : width(w), height(h), framebuffer(w * h * 3, 0) {}

    void render(const Scene& scene, const Camera& camera) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float u = (x + 0.5f) / width;
                float v = (y + 0.5f) / height;
                Ray ray = camera.getRay(u, v);
    
                Color finalColor(0.2f, 0.2f, 0.4f); // cor do fundo
                float closestT = std::numeric_limits<float>::max();
    
                // Verifica todos os objetos
                for (const auto& obj : scene.objects) {
                    HitInfo hit = obj->intersect(ray);
                    if (hit.hasHit && hit.t < closestT) {
                        closestT = hit.t;
                        finalColor = obj->getColor(hit);
                    }
                }
    
                int i = (y * width + x) * 3;
                framebuffer[i] = (unsigned char)(finalColor.r * 255);
                framebuffer[i + 1] = (unsigned char)(finalColor.g * 255);
                framebuffer[i + 2] = (unsigned char)(finalColor.b * 255);
            }
        }
    }
    

    const std::vector<unsigned char>& getFramebuffer() const {
        return framebuffer;
    }
};
