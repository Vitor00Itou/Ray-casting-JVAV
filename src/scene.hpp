// scene.hpp
#pragma once
#include "sphere.hpp"
#include "plane.hpp"
#include "light.hpp"
#include "object.hpp"
#include <vector>
#include <memory> 

struct Scene {
    std::vector<Object*> objects;
    std::vector<Light> lightSources;

    ~Scene() {
        for (auto obj : objects) {
            delete obj;
        }
    }
};

