#pragma once

#include "sphere.hpp"
#include "plane.hpp"
#include "light_point.hpp"
#include "object.hpp"
#include "box.hpp"
#include <vector>
#include <memory>
#include <iostream>

/**
 * @brief Represents a 3D scene containing objects, light sources, and active selection states.
 */
struct Scene {
    std::vector<Object*> objects;
    int currentObj = 0;

    ~Scene() {
        for (auto obj : objects) {
            delete obj;
        }
        objects.clear();
    }

    /**
     * @brief Retrieve pointers to all light emitter objects in the scene.
     */
    std::vector<Object*> getEmitters() const {
        std::vector<Object*> emitters;
        for (const auto& obj : objects) {
            if (obj && obj->isEmitter()) {
                emitters.push_back(obj);
            }
        }
        return emitters;
    }

    void nextObj() {
        if (objects.empty()) return;
        currentObj = (currentObj + 1) % static_cast<int>(objects.size());
    }

    void previousObj() {
        if (objects.empty()) return;
        currentObj = (currentObj - 1 + static_cast<int>(objects.size())) % static_cast<int>(objects.size());
    }

    Object* getCurrentObject() const {
        if (currentObj >= 0 && currentObj < static_cast<int>(objects.size())) {
            return objects[currentObj];
        }
        return nullptr;
    }

    // Object movement methods using polymorphic virtual dispatch
    void moveCurrentObjUp() {
        if (auto* obj = getCurrentObject()) obj->move(Vec3(0.0f, 0.1f, 0.0f));
    }

    void moveCurrentObjDown() {
        if (auto* obj = getCurrentObject()) obj->move(Vec3(0.0f, -0.1f, 0.0f));
    }

    void moveCurrentObjFront() {
        if (auto* obj = getCurrentObject()) obj->move(Vec3(0.0f, 0.0f, 0.1f));
    }

    void moveCurrentObjBack() {
        if (auto* obj = getCurrentObject()) obj->move(Vec3(0.0f, 0.0f, -0.1f));
    }

    void moveCurrentObjLeft() {
        if (auto* obj = getCurrentObject()) obj->move(Vec3(0.1f, 0.0f, 0.0f));
    }

    void moveCurrentObjRight() {
        if (auto* obj = getCurrentObject()) obj->move(Vec3(-0.1f, 0.0f, 0.0f));
    }

    // Luminosity manipulation methods using polymorphic virtual dispatch
    void addToLuminosity(float step) {
        if (auto* obj = getCurrentObject()) obj->addLuminosity(Color(step, step, step));
    }

    void addToLuminosityR(float step) {
        if (auto* obj = getCurrentObject()) obj->addLuminosity(Color(step, 0.0f, 0.0f));
    }

    void addToLuminosityG(float step) {
        if (auto* obj = getCurrentObject()) obj->addLuminosity(Color(0.0f, step, 0.0f));
    }

    void addToLuminosityB(float step) {
        if (auto* obj = getCurrentObject()) obj->addLuminosity(Color(0.0f, 0.0f, step));
    }
};


