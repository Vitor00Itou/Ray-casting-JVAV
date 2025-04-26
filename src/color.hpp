#pragma once

struct Color {
    float r, g, b;

    Color() : r(1.0), g(1.0), b(1.0) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    Color operator*(float s) const { return Color(r * s, g * s, b * s); }
};