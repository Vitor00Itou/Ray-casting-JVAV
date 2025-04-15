// scene.hpp
#pragma once
#include "sphere.hpp"
#include "light.hpp"
#include <vector>

struct Scene {
    std::vector<Sphere> objects;
    std::vector<Light> lightSources;
};
