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

                Color color(0.2f, 0.2f, 0.4f); // fundo

                // hitou blz
                // novo raio do hit pra luz
                // verificar interseção do novo raio com todos os obj
                //      se hitar em algum, já era SOMBRA
                //      se não, é LUZ
                //          sendo luz, pegar a contribuição de luz ()

                Ray shadowRay;
                Light light = scene.lightSources.at(0);
                for (const auto& obj : scene.objects) {
                    auto hit = obj.intersect(ray);
                    if (hit.hasHit) {
                        shadowRay = Ray(hit.point, light.position);
                        break;
                    }
                }

                for (const auto& obj : scene.objects) {
                    auto hit = obj.intersect(shadowRay);
                    if (hit.hasHit) {
                        color = Color(0,0,0); // se o shadow ray bater em alguma coisa, é PRETO
                    } else {
                        color = obj.color;
                    }
                }

                int i = (y * width + x) * 3;
                framebuffer[i] = (unsigned char)(color.r * 255);
                framebuffer[i + 1] = (unsigned char)(color.g * 255);
                framebuffer[i + 2] = (unsigned char)(color.b * 255);
            }
        }
    }

    const std::vector<unsigned char>& getFramebuffer() const {
        return framebuffer;
    }
};
