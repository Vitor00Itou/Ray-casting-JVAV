#pragma once
#include "scene.hpp"
#include "camera.hpp"
#include <vector>
#include <algorithm>

struct RayCastingRenderer {
    int width, height;
    std::vector<unsigned char> framebuffer;

    RayCastingRenderer(int w, int h) : width(w), height(h), framebuffer(w * h * 3, 0) {}

    void render(const Scene& scene, const Camera& camera) {
        std::vector<Object*> LightSources = scene.getEmitters();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float u = (x + 0.5f) / width;
                float v = (y + 0.5f) / height;
                Ray ray = camera.getRay(u, v);

                Color color(0.2f, 0.2f, 0.4f); // fundo
                float closestT = std::numeric_limits<float>::max();

                // hitou blz
                // novo raio do hit pra luz
                // verificar interseção do novo raio com todos os obj
                //      se hitar em algum, já era SOMBRA
                //      se não, é LUZ
                //          sendo luz, pegar a contribuição de luz ()

                Ray shadowRay;

                // Verifica objetos
                for (const auto& obj : scene.objects) {
                    auto hit = obj->intersect(ray);
                    if (hit.hasHit && hit.t < closestT) {
                        // Novo menor
                        closestT = hit.t;

                        Color intensityAccum(0.0, 0.0, 0.0);
                        if (obj->isEmitter()) {
                            intensityAccum = obj->getIntensity();
                        }

                        for(const auto& light : LightSources){
                            // Acha o cosseno do angulo entre a luz e a normal
                            Vec3 lightDir = light->getLightDir(hit);
                            float cosAngNormalLight = hit.normal.dot(lightDir); //pega o angulo entre a normal da superficie e a direção da luz
                            
                            // Calcula a intensidade da luz a ser exibida
                            Color hitLightIntensity = light->getIntensity() * cosAngNormalLight;
    
                            // Se a normal está apontando PARA TRÁS da luz, está em sombra
                            if (cosAngNormalLight <= 0.0f) {
                                hitLightIntensity = Color(0, 0, 0); // Escuro total (backface)
                            }
                            // Caso contrário, testa sombra normalmente
                            else {
                                Vec3 shadowRayOrigin = hit.point + (hit.normal * 0.01f); // Bias
                                Vec3 shadowRayDir = lightDir; // Já normalizada
                                shadowRay = Ray(shadowRayOrigin, shadowRayDir);

                                //Acha a distância entre o ponto e a luz
                                float maxDistance = (light->getCenter() - shadowRayOrigin).length();
                    
                                bool inShadow = false;
    
                                // Checa sombra com objetos
                                for (const auto& otherObj : scene.objects) {
                                    if (otherObj == obj) {
                                        continue; // Ignora o próprio objeto
                                    }
                                    if (otherObj == light) {
                                        continue; // Ignora a superfície do objeto luminoso
                                    }
                                    
                                    //Calcula a interceção do objeto com o shadowRay
                                    HitInfo shadowHit = otherObj->intersect(shadowRay);

                                    if (shadowHit.hasHit && shadowHit.t < maxDistance) {
                                        inShadow = true;
                                        break;
                                    }
                                }
    
                                if (inShadow) {
                                    hitLightIntensity = Color(0, 0, 0); // Sombra
                                }
                            }

                            // Acumula as intensidades da luz no ponto
                            intensityAccum.r += hitLightIntensity.r;
                            intensityAccum.g += hitLightIntensity.g;
                            intensityAccum.b += hitLightIntensity.b;

                        }
                        intensityAccum.r = std::clamp(intensityAccum.r, 0.0f, 1.0f);
                        intensityAccum.g = std::clamp(intensityAccum.g, 0.0f, 1.0f);
                        intensityAccum.b = std::clamp(intensityAccum.b, 0.0f, 1.0f);
                        
                        // Aplica as intensidades no pixel
                        color.r = obj->getColor(hit).r * intensityAccum.r;
                        color.g = obj->getColor(hit).g * intensityAccum.g;
                        color.b = obj->getColor(hit).b * intensityAccum.b;
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
