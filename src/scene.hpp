// scene.hpp
#pragma once
#include "sphere.hpp"
#include "plane.hpp"
#include "light_point.hpp"
#include "object.hpp"
#include "box.hpp"
#include <vector>
#include <memory>
#include <iostream> 

struct Scene {
    std::vector<Object*> objects;
    int currentObj = 0;

    std::vector<Object*> getEmitters() const {
        std::vector<Object*> emitters;
        for (auto& obj : objects) {
            if (obj->isEmitter()) {
                emitters.push_back(obj);
            }
        }
        return emitters;
    }

    ~Scene() {
        for (auto obj : objects) {
            delete obj;
        }
    }

    void nextObj() {
        currentObj = (currentObj + 1) % objects.size();
    }

    void previousObj() {
        currentObj = (currentObj - 1) % objects.size();
    }

    void moveCurrentObjUp() {
        Object* obj = objects[currentObj];
        switch (obj->type) {
            case SPHERE: {
                Sphere* sphere = dynamic_cast<Sphere*>(obj);
                if (sphere) {
                    sphere->center.y += 0.1;
                }
                break;
            }

            case PLANE: {
                Plane* plane = dynamic_cast<Plane*>(obj);
                if (plane) {
                    plane->point.y += 0.1;
                }
                break;
            }

            case BOX: {
                Box* box = dynamic_cast<Box*>(obj);
                if (box) {
                    box->minCorner.y += 0.1;
                    box->maxCorner.y += 0.1;
                }
                break;
            }

            case LIGHT_POINT: {
                LightPoint* lightPoint = dynamic_cast<LightPoint*>(obj);
                if (lightPoint) {
                    lightPoint->position.y += 0.1;

                }
                break;
            }
        
            default:
                break;
        }
    }

    void moveCurrentObjDown() {
        Object* obj = objects[currentObj];
        switch (obj->type) {
            case SPHERE: {
                Sphere* sphere = dynamic_cast<Sphere*>(obj);
                if (sphere) {
                    sphere->center.y -= 0.1;
                }
                break;
            }

            case PLANE: {
                Plane* plane = dynamic_cast<Plane*>(obj);
                if (plane) {
                    plane->point.y -= 0.1;
                }
                break;
            }

            case BOX: {
                Box* box = dynamic_cast<Box*>(obj);
                if (box) {
                    box->minCorner.y -= 0.1;
                    box->maxCorner.y -= 0.1;
                }
                break;
            }

            case LIGHT_POINT: {
                LightPoint* lightPoint = dynamic_cast<LightPoint*>(obj);
                if (lightPoint) {
                    lightPoint->position.y -= 0.1;

                }
                break;
            }
        
            default:
                break;
        }
    }

    void moveCurrentObjFront() {
        Object* obj = objects[currentObj];
        switch (obj->type) {
            case SPHERE: {
                Sphere* sphere = dynamic_cast<Sphere*>(obj);
                if (sphere) {
                    sphere->center.z += 0.1;
                }
                break;
            }

            case PLANE: {
                Plane* plane = dynamic_cast<Plane*>(obj);
                if (plane) {
                    plane->point.z += 0.1;
                }
                break;
            }

            case BOX: {
                Box* box = dynamic_cast<Box*>(obj);
                if (box) {
                    box->minCorner.z += 0.1;
                    box->maxCorner.z += 0.1;
                }
                break;
            }

            case LIGHT_POINT: {
                LightPoint* lightPoint = dynamic_cast<LightPoint*>(obj);
                if (lightPoint) {
                    lightPoint->position.z += 0.1;

                }
                break;
            }
        
            default:
                break;
        }
    }

    void moveCurrentObjBack() {
        Object* obj = objects[currentObj];
        switch (obj->type) {
            case SPHERE: {
                Sphere* sphere = dynamic_cast<Sphere*>(obj);
                if (sphere) {
                    sphere->center.z -= 0.1;
                }
                break;
            }

            case PLANE: {
                Plane* plane = dynamic_cast<Plane*>(obj);
                if (plane) {
                    plane->point.z -= 0.1;
                }
                break;
            }

            case BOX: {
                Box* box = dynamic_cast<Box*>(obj);
                if (box) {
                    box->minCorner.z -= 0.1;
                    box->maxCorner.z -= 0.1;
                }
                break;
            }

            case LIGHT_POINT: {
                LightPoint* lightPoint = dynamic_cast<LightPoint*>(obj);
                if (lightPoint) {
                    lightPoint->position.z -= 0.1;

                }
                break;
            }
        
            default:
                break;
        }
    }

    void moveCurrentObjLeft() {
        Object* obj = objects[currentObj];
        switch (obj->type) {
            case SPHERE: {
                Sphere* sphere = dynamic_cast<Sphere*>(obj);
                if (sphere) {
                    sphere->center.x += 0.1;
                }
                break;
            }

            case PLANE: {
                Plane* plane = dynamic_cast<Plane*>(obj);
                if (plane) {
                    plane->point.x += 0.1;
                }
                break;
            }

            case BOX: {
                Box* box = dynamic_cast<Box*>(obj);
                if (box) {
                    box->minCorner.x += 0.1;
                    box->maxCorner.x += 0.1;
                }
                break;
            }

            case LIGHT_POINT: {
                LightPoint* lightPoint = dynamic_cast<LightPoint*>(obj);
                if (lightPoint) {
                    lightPoint->position.x += 0.1;

                }
                break;
            }
        
            default:
                break;
        }
    }

    void moveCurrentObjRight() {
        Object* obj = objects[currentObj];
        switch (obj->type) {
            case SPHERE: {
                Sphere* sphere = dynamic_cast<Sphere*>(obj);
                if (sphere) {
                    sphere->center.x -= 0.1;
                }
                break;
            }

            case PLANE: {
                Plane* plane = dynamic_cast<Plane*>(obj);
                if (plane) {
                    plane->point.x -= 0.1;
                }
                break;
            }

            case BOX: {
                Box* box = dynamic_cast<Box*>(obj);
                if (box) {
                    box->minCorner.x -= 0.1;
                    box->maxCorner.x -= 0.1;
                }
                break;
            }

            case LIGHT_POINT: {
                LightPoint* lightPoint = dynamic_cast<LightPoint*>(obj);
                if (lightPoint) {
                    lightPoint->position.x -= 0.1;

                }
                break;
            }
        
            default:
                break;
        }
    }

    void addToLuminosity(float luminosityIncrease) {
        Object* obj = objects[currentObj];
        switch (obj->type) {
            case SPHERE: {
                Sphere* sphere = dynamic_cast<Sphere*>(obj);
                if (sphere && sphere->isEmitter()) {
                    sphere->color = sphere->color + Color(luminosityIncrease, luminosityIncrease, luminosityIncrease);
                }
                break;
            }

            case PLANE: {
                Plane* plane = dynamic_cast<Plane*>(obj);
                if (plane && plane->isEmitter()) {
                    plane->color = plane->color + Color(luminosityIncrease, luminosityIncrease, luminosityIncrease);
                }
                break;
            }

            case BOX: {
                Box* box = dynamic_cast<Box*>(obj);
                if (box && box->isEmitter()) {
                    box->color = box->color + Color(luminosityIncrease, luminosityIncrease, luminosityIncrease);
                }
                break;
            }

            case LIGHT_POINT: {
                LightPoint* lightPoint = dynamic_cast<LightPoint*>(obj);
                if (lightPoint) {
                    lightPoint->lightColor = lightPoint->lightColor + Color(luminosityIncrease, luminosityIncrease, luminosityIncrease);;

                }
                break;
            }
        
            default:
                break;
        }
    }

    void addToLuminosityR(float luminosityIncrease) {
        Object* obj = objects[currentObj];
        switch (obj->type) {
            case SPHERE: {
                Sphere* sphere = dynamic_cast<Sphere*>(obj);
                if (sphere && sphere->isEmitter()) {
                    sphere->color = sphere->color + Color(luminosityIncrease, 0, 0);
                }
                break;
            }

            case PLANE: {
                Plane* plane = dynamic_cast<Plane*>(obj);
                if (plane && plane->isEmitter()) {
                    plane->color = plane->color + Color(luminosityIncrease, 0, 0);
                }
                break;
            }

            case BOX: {
                Box* box = dynamic_cast<Box*>(obj);
                if (box && box->isEmitter()) {
                    box->color = box->color + Color(luminosityIncrease, 0, 0);
                }
                break;
            }

            case LIGHT_POINT: {
                LightPoint* lightPoint = dynamic_cast<LightPoint*>(obj);
                if (lightPoint) {
                    lightPoint->lightColor = lightPoint->lightColor + Color(luminosityIncrease, 0, 0);;

                }
                break;
            }
        
            default:
                break;
        }
    }

    void addToLuminosityG(float luminosityIncrease) {
        Object* obj = objects[currentObj];
        switch (obj->type) {
            case SPHERE: {
                Sphere* sphere = dynamic_cast<Sphere*>(obj);
                if (sphere && sphere->isEmitter()) {
                    sphere->color = sphere->color + Color(0, luminosityIncrease, 0);
                }
                break;
            }

            case PLANE: {
                Plane* plane = dynamic_cast<Plane*>(obj);
                if (plane && plane->isEmitter()) {
                    plane->color = plane->color + Color(0, luminosityIncrease, 0);
                }
                break;
            }

            case BOX: {
                Box* box = dynamic_cast<Box*>(obj);
                if (box && box->isEmitter()) {
                    box->color = box->color + Color(0, luminosityIncrease, 0);
                }
                break;
            }

            case LIGHT_POINT: {
                LightPoint* lightPoint = dynamic_cast<LightPoint*>(obj);
                if (lightPoint) {
                    lightPoint->lightColor = lightPoint->lightColor + Color(0, luminosityIncrease, 0);;

                }
                break;
            }
        
            default:
                break;
        }
    }

    void addToLuminosityB(float luminosityIncrease) {
        Object* obj = objects[currentObj];
        switch (obj->type) {
            case SPHERE: {
                Sphere* sphere = dynamic_cast<Sphere*>(obj);
                if (sphere && sphere->isEmitter()) {
                    sphere->color = sphere->color + Color(0, 0, luminosityIncrease);
                }
                break;
            }

            case PLANE: {
                Plane* plane = dynamic_cast<Plane*>(obj);
                if (plane && plane->isEmitter()) {
                    plane->color = plane->color + Color(0, 0, luminosityIncrease);
                }
                break;
            }

            case BOX: {
                Box* box = dynamic_cast<Box*>(obj);
                if (box && box->isEmitter()) {
                    box->color = box->color + Color(0, 0, luminosityIncrease);
                }
                break;
            }

            case LIGHT_POINT: {
                LightPoint* lightPoint = dynamic_cast<LightPoint*>(obj);
                if (lightPoint) {
                    lightPoint->lightColor = lightPoint->lightColor + Color(0, 0, luminosityIncrease);;

                }
                break;
            }
        
            default:
                break;
        }
    }
};

