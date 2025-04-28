#pragma once
#include "scene.hpp"
#include "camera.hpp"
#include <vector>
#include <algorithm>

struct RayCastingRenderer {
    int width, height;
    std::vector<unsigned char> framebuffer;

    RayCastingRenderer(int w, int h) : width(w), height(h), framebuffer(w * h * 3, 0) {}

    // Cálculo de reflexão
    Vec3 reflect(const Vec3& incident, const Vec3& normal) {
        return incident - normal * 2.0f * incident.dot(normal);
    }

    void render(const Scene& scene, const Camera& camera, int maxRecursionDepth) {
        std::vector<Object*> LightSources = scene.getEmitters();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float u = (x + 0.5f) / width;
                float v = (y + 0.5f) / height;
                Ray ray = camera.getRay(u, v);

                Color color = castRay(ray, scene, LightSources, 0, maxRecursionDepth);

                int i = (y * width + x) * 3;
                framebuffer[i] = (unsigned char)(color.r * 255);
                framebuffer[i + 1] = (unsigned char)(color.g * 255);
                framebuffer[i + 2] = (unsigned char)(color.b * 255);
            }
        }
    }

    Color castRay(const Ray& rayCasted, const Scene& scene, const std::vector<Object*>& LightSources, int recursionDepth, int maxRecursionDepth) {
        Color reflectionColor(0.2f, 0.2f, 0.4f); // fundo
        Color color(0.2f, 0.2f, 0.4f); // fundo
        float closestT = std::numeric_limits<float>::max();

        Ray shadowRay;

        // Verifica objetos
        for (const auto& obj : scene.objects) {
            auto hit = obj->intersect(rayCasted);
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
                        float transparencyFactor = 1.0f;

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
                            
                            // Atingiu o objeto
                            if (shadowHit.hasHit && shadowHit.t < maxDistance) {
                                if (otherObj->isTransparent()) {
                                    // Se o objeto for transparente, acumula a sua transparencia
                                    transparencyFactor *= otherObj->getTransparency();
                                    // Continua checando outros objetos
                                }else{
                                    inShadow = true;
                                    break;  
                                }
                            }
                        }
                        #include <iostream>
                        
                        hitLightIntensity = hitLightIntensity*transparencyFactor;

                        if (inShadow) {
                            hitLightIntensity = Color(0, 0, 0); // Sombra
                        }
                    }

                    if (hitLightIntensity.r > 0.0f || hitLightIntensity.g > 0.0f || hitLightIntensity.b > 0.0f) { // só calcula especular se tiver difusa também
                        // Direção da câmera (olho para o ponto)
                        Vec3 viewDir = (rayCasted.origin - hit.point).normalize();

                        // Vetor metade (entre direção da luz e da câmera)
                        Vec3 halfVector = (lightDir + viewDir).normalize();

                        // Produto escalar da normal com o vetor metade
                        float cosAngNormalHalf = hit.normal.dot(halfVector);

                        // Se for negativo, zero
                        cosAngNormalHalf = std::max(0.0f, cosAngNormalHalf);

                        // Coeficiente especular
                        float shininess = obj->getSpecularShininess(); // <- cada objeto pode ter o seu!
                        float specularFactor = pow(cosAngNormalHalf, shininess);
                
                        Color specularColor = light->getIntensity() * specularFactor;
                
                        hitLightIntensity.r += specularColor.r;
                        hitLightIntensity.g += specularColor.g;
                        hitLightIntensity.b += specularColor.b;
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

                // Se o objeto for um espelho, refletir o raio
                if (obj->isReflective()) {
                    // Cálculo da reflexão (recursão de reflexões)
                    Vec3 reflectDir = reflect(rayCasted.direction, hit.normal);
                    Ray reflectRay(hit.point + hit.normal * 0.01f, reflectDir); // Bias e direção da reflexão

                    if(recursionDepth < maxRecursionDepth){
                        recursionDepth++;
                        float reflectionCoefficient = obj->getReflectionCoefficient();
                        color = color * (1.0f - reflectionCoefficient) +  castRay(reflectRay, scene, LightSources, recursionDepth, maxRecursionDepth) * reflectionCoefficient;
                    }
                    
                // Se o objeto for transparente, refratar o raio
                } 
                if (obj->isTransparent()) {
                    float eta = obj->getRefractiveIndex(); 
                    Vec3 refractDir = refract(rayCasted.direction, hit.normal, eta).normalize();
                    Ray refractRay(hit.point - hit.normal * 0.01f, refractDir); // pequeno bias para evitar reinterseção
                
                    if (recursionDepth < maxRecursionDepth) {
                        recursionDepth++;
                        Color transmissionColor = castRay(refractRay, scene, LightSources, recursionDepth, maxRecursionDepth);
                
                        float transparency = obj->getTransparency(); // tipo 0.8 para vidro quase invisível
                        color = color * (1.0f - transparency) + transmissionColor * transparency;
                    }
                }
            }
        }
        return color;
    }

    Vec3 refract(const Vec3& incident, const Vec3& normal, float eta) {
        if (fabs(eta - 1.0f) < 1e-6f) {
            return incident; // nada muda
        }
        float cosi = std::clamp(incident.dot(normal), -1.0f, 1.0f);
        float etai = 1.0f, etat = eta;
        Vec3 n = normal;
        if (cosi < 0) {
            cosi = -cosi;
        } else {
            std::swap(etai, etat);
            n = normal*-1;
        }
        float etaRatio = etai / etat;
        float k = 1 - etaRatio * etaRatio * (1 - cosi * cosi);
        if (k < 0) {
            return reflect(incident, normal); // reflexão total interna
        } else {
            return incident * etaRatio + n * (etaRatio * cosi - sqrtf(k));
        }
    }

    const std::vector<unsigned char>& getFramebuffer() const {
        return framebuffer;
    }
};
