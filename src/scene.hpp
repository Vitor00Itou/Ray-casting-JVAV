// scene.hpp
#pragma once
#include "sphere.hpp"
#include "plane.hpp"
#include "light.hpp"
#include <vector>

struct Scene {
    std::vector<Sphere> objects;
    std::vector<Plane> planes;
    std::vector<Light> lightSources;
};
