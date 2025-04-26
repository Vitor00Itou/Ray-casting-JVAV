// scene.hpp
#pragma once
#include "sphere.hpp"
#include "plane.hpp"
#include "light_point.hpp"
#include "object.hpp"
#include <vector>
#include <memory> 

struct Scene {
    std::vector<Object*> objects;

    std::vector<Object*> getEmitters() const {
        std::vector<Object*> emitters;
        for (auto& obj : objects) {
            if (obj->isEmitter()) emitters.push_back(obj);
        }
        return emitters;
    }

    ~Scene() {
        for (auto obj : objects) {
            delete obj;
        }
    }
};

