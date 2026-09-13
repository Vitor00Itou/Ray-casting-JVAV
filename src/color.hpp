#pragma once

#include <algorithm>

/**
 * @brief RGB Color representation with floating-point components (0.0 to 1.0).
 */
struct Color {
    float r, g, b;

    Color() : r(1.0f), g(1.0f), b(1.0f) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    Color operator*(float s) const { return Color(r * s, g * s, b * s); }
    Color operator*(const Color& c) const { return Color(r * c.r, g * c.g, b * c.b); }
    Color operator+(const Color& c) const { return Color(r + c.r, g + c.g, b + c.b); }

    // Clamp RGB values to [0.0, 1.0] range
    Color clamp() const {
        return Color(
            std::clamp(r, 0.0f, 1.0f),
            std::clamp(g, 0.0f, 1.0f),
            std::clamp(b, 0.0f, 1.0f)
        );
    }
};
