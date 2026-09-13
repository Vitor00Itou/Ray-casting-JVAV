#pragma once

#include "scene.hpp"
#include "camera.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

/**
 * @brief Whitted-style Ray Tracing Engine.
 * Supports Phong shading model (ambient, diffuse, specular), hard/soft shadows,
 * recursive specular reflection, and Snell's law refractive transmission with total internal reflection.
 */
struct RayCastingRenderer {
    int width;
    int height;
    std::vector<unsigned char> framebuffer;

    RayCastingRenderer(int w, int h) : width(w), height(h), framebuffer(w * h * 3, 0) {}

    // Vector reflection calculation R = I - 2*(I.N)*N
    Vec3 reflect(const Vec3& incident, const Vec3& normal) const {
        return incident - normal * 2.0f * incident.dot(normal);
    }

    // Vector refraction calculation using Snell's Law
    Vec3 refract(const Vec3& incident, const Vec3& normal, float eta) const {
        if (std::abs(eta - 1.0f) < 1e-6f) {
            return incident;
        }
        float cosi = std::clamp(incident.dot(normal), -1.0f, 1.0f);
        float etai = 1.0f, etat = eta;
        Vec3 n = normal;
        if (cosi < 0.0f) {
            cosi = -cosi;
        } else {
            std::swap(etai, etat);
            n = normal * (-1.0f);
        }
        float etaRatio = etai / etat;
        float k = 1.0f - etaRatio * etaRatio * (1.0f - cosi * cosi);
        if (k < 0.0f) {
            return reflect(incident, normal); // Total Internal Reflection (TIR)
        } else {
            return incident * etaRatio + n * (etaRatio * cosi - std::sqrt(k));
        }
    }

    /**
     * @brief Render scene from camera viewpoint into internal RGB framebuffer.
     */
    void render(const Scene& scene, const Camera& camera, int maxRecursionDepth) {
        std::vector<Object*> lightSources = scene.getEmitters();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float u = (x + 0.5f) / static_cast<float>(width);
                float v = (y + 0.5f) / static_cast<float>(height);
                Ray ray = camera.getRay(u, v);

                Color color = castRay(ray, scene, lightSources, 0, maxRecursionDepth);

                int i = (y * width + x) * 3;
                framebuffer[i]     = static_cast<unsigned char>(std::clamp(color.r, 0.0f, 1.0f) * 255.0f);
                framebuffer[i + 1] = static_cast<unsigned char>(std::clamp(color.g, 0.0f, 1.0f) * 255.0f);
                framebuffer[i + 2] = static_cast<unsigned char>(std::clamp(color.b, 0.0f, 1.0f) * 255.0f);
            }
        }
    }

    /**
     * @brief Recursive ray payload evaluation.
     */
    Color castRay(const Ray& rayCasted, const Scene& scene, const std::vector<Object*>& lightSources, int recursionDepth, int maxRecursionDepth) const {
        Color background(0.2f, 0.2f, 0.4f);
        Color color = background;
        float closestT = std::numeric_limits<float>::max();

        for (const auto& obj : scene.objects) {
            if (!obj) continue;
            auto hit = obj->intersect(rayCasted);
            if (hit.hasHit && hit.t < closestT) {
                closestT = hit.t;

                // Inert objects render self color directly without lighting calculation
                if (obj->isInert()) {
                    color = obj->getColor(hit);
                    continue;
                }

                Color intensityAccum(0.0f, 0.0f, 0.0f);
                if (obj->isEmitter()) {
                    intensityAccum = obj->getIntensity();
                }

                for (const auto& light : lightSources) {
                    if (!light) continue;
                    Vec3 lightDir = light->getLightDir(hit);
                    float cosAngNormalLight = hit.normal.dot(lightDir);

                    Color hitLightIntensity(0.0f, 0.0f, 0.0f);

                    if (cosAngNormalLight > 0.0f) {
                        hitLightIntensity = light->getIntensity() * cosAngNormalLight;

                        // Shadow ray test
                        Vec3 shadowRayOrigin = hit.point + (hit.normal * 0.001f);
                        Ray shadowRay(shadowRayOrigin, lightDir);

                        float maxDistance = (light->getCenter() - shadowRayOrigin).length();
                        bool inShadow = false;
                        float transparencyFactor = 1.0f;

                        for (const auto& otherObj : scene.objects) {
                            if (!otherObj || otherObj == obj || otherObj == light) continue;

                            HitInfo shadowHit = otherObj->intersect(shadowRay);
                            if (shadowHit.hasHit && shadowHit.t < maxDistance) {
                                if (otherObj->isTransparent()) {
                                    transparencyFactor *= otherObj->getTransparency();
                                } else {
                                    inShadow = true;
                                    break;
                                }
                            }
                        }

                        hitLightIntensity = hitLightIntensity * transparencyFactor;
                        if (inShadow) {
                            hitLightIntensity = Color(0.0f, 0.0f, 0.0f);
                        }
                    }

                    // Phong Specular component calculation
                    if (hitLightIntensity.r > 0.0f || hitLightIntensity.g > 0.0f || hitLightIntensity.b > 0.0f) {
                        Vec3 viewDir = (rayCasted.origin - hit.point).normalize();
                        Vec3 halfVector = (lightDir + viewDir).normalize();

                        float cosAngNormalHalf = std::max(0.0f, hit.normal.dot(halfVector));
                        float shininess = obj->getSpecularShininess();
                        float specularFactor = std::pow(cosAngNormalHalf, shininess);

                        Color specularColor = light->getIntensity() * specularFactor;
                        hitLightIntensity = hitLightIntensity + specularColor;
                    }

                    intensityAccum = intensityAccum + hitLightIntensity;
                }

                intensityAccum = intensityAccum.clamp();

                Color objColor = obj->getColor(hit);
                color = objColor * intensityAccum;

                // Recursive reflection
                if (obj->isReflective() && recursionDepth < maxRecursionDepth) {
                    Vec3 reflectDir = reflect(rayCasted.direction, hit.normal);
                    Ray reflectRay(hit.point + hit.normal * 0.001f, reflectDir);
                    float reflCoeff = obj->getReflectionCoefficient();
                    Color reflColor = castRay(reflectRay, scene, lightSources, recursionDepth + 1, maxRecursionDepth);
                    color = color * (1.0f - reflCoeff) + reflColor * reflCoeff;
                }

                // Recursive refraction transmission
                if (obj->isTransparent() && recursionDepth < maxRecursionDepth) {
                    float eta = obj->getRefractiveIndex();
                    Vec3 refractDir = refract(rayCasted.direction, hit.normal, eta).normalize();
                    Ray refractRay(hit.point - hit.normal * 0.001f, refractDir);
                    Color transmissionColor = castRay(refractRay, scene, lightSources, recursionDepth + 1, maxRecursionDepth);
                    float transp = obj->getTransparency();
                    color = color * (1.0f - transp) + transmissionColor * transp;
                }
            }
        }
        return color;
    }

    const std::vector<unsigned char>& getFramebuffer() const {
        return framebuffer;
    }
};

