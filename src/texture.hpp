#pragma once

#include <stdexcept>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "color.hpp"

struct Texture {
    int width;
    int height;
    int channels;
    unsigned char* data;

	Texture () {}

    Texture (const char* filename, Color color) : width(1), height(1), channels(3){
        data = stbi_load(filename, &width, &height, &channels, 0);
        if (!data) {
            data = new unsigned char[channels];
            unsigned char R = static_cast<unsigned char>(color.r * 255);
            unsigned char G = static_cast<unsigned char>(color.g * 255);
            unsigned char B = static_cast<unsigned char>(color.b * 255);
            data[0] = R;
            data[1] = G;
            data[2] = B;
        }
    }

    Texture (const char* filename) {
        data = stbi_load(filename, &width, &height, &channels, 0);
        if (!data) {
            throw std::runtime_error("Error while loading image");
        }
    }

	// Construtor de textura sólida
    Texture (Color color) : width(1), height(1), channels(3) {
        data = new unsigned char[channels];
        unsigned char R = static_cast<unsigned char>(color.r * 255);
        unsigned char G = static_cast<unsigned char>(color.g * 255);
        unsigned char B = static_cast<unsigned char>(color.b * 255);
        data[0] = R;
        data[1] = G;
        data[2] = B;

    }

    // retorna cor normalizada (float entre 0.0 e 1.0)
	// 'u' e 'v' são coordenadas normalizadas em relação a superficie do objeto
    Color getColorFromImgCoordinates (SurfaceCoord surfaceCoord) const {
        int x = std::clamp(int(surfaceCoord.u * width), 0, width - 1);
        int y = std::clamp(int(surfaceCoord.v * height), 0, height - 1);
        int index = (y * width + x) * channels;
        float r = data[index]     / 255.0f;
        float g = data[index + 1] / 255.0f;
        float b = data[index + 2] / 255.0f;
		return Color{r, g, b};
    }
};
