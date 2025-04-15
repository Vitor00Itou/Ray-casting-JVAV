#pragma once
#include <cmath>

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    
    float angle(Vec3& v1, Vec3& v2) {
        float dot = v1.dot(v2);
        float normA = v1.norm();
        float normB = v2.norm();
        return acos(dot / (normA * normB));
    }
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 normalize() const {
        float len = std::sqrt(x*x + y*y + z*z);
        return (len > 0) ? (*this * (1.0f / len)) : Vec3();
    }
    float norm() {
        return sqrt(x * x + y * y + z * z);
    }
};