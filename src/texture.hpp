#pragma once

#include <stdexcept>
#include <algorithm>
#include <memory>
#include <iostream>

#include "stb_image.h"
#include "color.hpp"
#include "hitinfo.hpp"

/**
 * @brief Manages 2D image textures and solid color textures for ray casting objects.
 */
struct Texture {
    int width = 1;
    int height = 1;
    int channels = 3;
    std::shared_ptr<unsigned char> data;

    Texture() {
        data = std::shared_ptr<unsigned char>(new unsigned char[3]{255, 255, 255}, [](unsigned char* p) { delete[] p; });
    }

    // Solid color texture constructor
    Texture(Color color) : width(1), height(1), channels(3) {
        unsigned char r = static_cast<unsigned char>(std::clamp(color.r, 0.0f, 1.0f) * 255);
        unsigned char g = static_cast<unsigned char>(std::clamp(color.g, 0.0f, 1.0f) * 255);
        unsigned char b = static_cast<unsigned char>(std::clamp(color.b, 0.0f, 1.0f) * 255);

        unsigned char* rawData = new unsigned char[3]{r, g, b};
        data = std::shared_ptr<unsigned char>(rawData, [](unsigned char* p) { delete[] p; });
    }

    // Image texture constructor with fallback solid color
    Texture(const char* filename, Color fallbackColor) : width(1), height(1), channels(3) {
        if (filename && filename[0] != '\0') {
            int w, h, c;
            unsigned char* imgData = stbi_load(filename, &w, &h, &c, 0);
            if (imgData) {
                width = w;
                height = h;
                channels = c;
                data = std::shared_ptr<unsigned char>(imgData, [](unsigned char* p) { stbi_image_free(p); });
                return;
            } else {
                std::cerr << "Warning: Failed to load texture '" << filename << "'. Falling back to solid color." << std::endl;
            }
        }
        *this = Texture(fallbackColor);
    }

    // Image texture constructor throwing exception if load fails
    Texture(const char* filename) {
        if (!filename || filename[0] == '\0') {
            *this = Texture(Color(1.0f, 1.0f, 1.0f));
            return;
        }
        int w, h, c;
        unsigned char* imgData = stbi_load(filename, &w, &h, &c, 0);
        if (!imgData) {
            throw std::runtime_error(std::string("Failed to load image texture: ") + filename);
        }
        width = w;
        height = h;
        channels = c;
        data = std::shared_ptr<unsigned char>(imgData, [](unsigned char* p) { stbi_image_free(p); });
    }

    /**
     * @brief Sample color from UV surface coordinates.
     */
    Color getColorFromImgCoordinates(SurfaceCoord surfaceCoord) const {
        if (!data) return Color(1.0f, 1.0f, 1.0f);

        int x = std::clamp(static_cast<int>(surfaceCoord.u * width), 0, width - 1);
        int y = std::clamp(static_cast<int>(surfaceCoord.v * height), 0, height - 1);
        int index = (y * width + x) * channels;

        const unsigned char* raw = data.get();
        float r = raw[index] / 255.0f;
        float g = (channels > 1) ? (raw[index + 1] / 255.0f) : r;
        float b = (channels > 2) ? (raw[index + 2] / 255.0f) : r;
        return Color(r, g, b);
    }
};

