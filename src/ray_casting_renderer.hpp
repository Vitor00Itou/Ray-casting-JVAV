#pragma once
#include "scene.hpp"
#include "camera.hpp"
#include <vector>

struct RayCastingRenderer {
    int width, height;
    std::vector<unsigned char> framebuffer;

    RayCastingRenderer(int w, int h) : width(w), height(h), framebuffer(w * h * 3, 0) {}

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
                        
                        // Acha o cosseno do angulo entre a luz e a normal
                        Vec3 lightDir = (light.position - hit.point).normalize();
                        float cosAngNormalLight = hit.normal.dot(lightDir);
                        
                        // Calcula a intensidade da luz a ser exibida
                        color = obj.texture.getColorFromImgCoordinates(hit.surfaceCoord);
                        color.b *= cosAngNormalLight * light.intensity;
                        color.r *= cosAngNormalLight * light.intensity;
                        color.g *= cosAngNormalLight * light.intensity;

                        // Se a normal está apontando PARA TRÁS da luz, está em sombra
                        if (cosAngNormalLight <= 0.0f) {
                            color = Color(0, 0, 0); // Escuro total (backface)
                        }
                        // Caso contrário, testa sombra normalmente
                        else {
                            Vec3 shadowRayOrigin = hit.point + (hit.normal * 0.01f); // Bias
                            Vec3 shadowRayDir = lightDir; // Já normalizada
                            shadowRay = Ray(shadowRayOrigin, shadowRayDir);
                
                            bool inShadow = false;
                            for (const auto& otherObj : scene.objects) {
                                if (&otherObj == &obj) continue; // Ignora o próprio objeto
                                if (otherObj.intersect(shadowRay).hasHit) {
                                    inShadow = true;
                                    break;
                                }
                            }
                            if (inShadow) {
                                color = Color(0, 0, 0); // Sombra
                            }
                        }
                        break;
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
