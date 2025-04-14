#pragma once
#include "scene.hpp"
#include "camera.hpp"
#include <vector>

struct Renderer {
    int width, height;
    std::vector<unsigned char> framebuffer;

    Renderer(int w, int h) : width(w), height(h), framebuffer(w * h * 3, 0) {}

    void render(const Scene& scene, const Camera& camera) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float u = (x + 0.5f) / width;
                float v = (y + 0.5f) / height;
                Ray ray = camera.getRay(u, v);

                float minT = 1e30f;
                Vec3 color(0.2f, 0.2f, 0.4f); // fundo

                for (const auto& obj : scene.objects) {
                    auto hit = obj.intersect(ray);
                    if (hit && hit->t < minT) {
                        minT = hit->t;
                        color = Vec3(1, 0, 0); // cor vermelha para esferas
                    }
                }

                int i = (y * width + x) * 3;
                framebuffer[i] = (unsigned char)(color.x * 255);
                framebuffer[i + 1] = (unsigned char)(color.y * 255);
                framebuffer[i + 2] = (unsigned char)(color.z * 255);
            }
        }
    }

    const std::vector<unsigned char>& getFramebuffer() const {
        return framebuffer;
    }
};
