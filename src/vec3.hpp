#pragma once

#include <cmath>
#include <algorithm>


/**
 * @brief 3D Vector representation supporting basic vector arithmetic, dot product,
 * cross product, normalization, and magnitude calculation.
 */
struct Vec3 {
    float x, y, z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Vector operations
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    Vec3 operator/(float s) const { return Vec3(x / s, y / s, z / s); }

    // Vector dot product
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }

    // Vector cross product
    Vec3 cross(const Vec3& v) const {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    // Magnitude / Length
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float norm() const {
        return length();
    }

    // Normalize vector (returns zero vector if length is near zero)
    Vec3 normalize() const {
        float len = length();
        return (len > 1e-6f) ? (*this * (1.0f / len)) : Vec3(0, 0, 0);
    }

    // Angle between two vectors in radians
    static float angle(const Vec3& v1, const Vec3& v2) {
        float normProduct = v1.norm() * v2.norm();
        if (normProduct < 1e-6f) return 0.0f;
        return std::acos(std::clamp(v1.dot(v2) / normProduct, -1.0f, 1.0f));
    }
};
