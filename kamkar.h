#pragma once
#include <GLFW/glfw3.h>
#include <noise.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

template <typename T>
T *emalloc(unsigned int count) {
    T *const result = static_cast<T *>(malloc(count * sizeof(T)));
    if (result == nullptr) {
        perror("Allocation with calloc failed");
    }
    return result;
}

template <typename T>
T *ecalloc(unsigned int count) {
    T *const result = static_cast<T *>(calloc(count, sizeof(T)));
    if (result == nullptr) {
        perror("Allocation with malloc failed");
    }
    return result;
}

constexpr float sqrt3 = 1.73205080757f;
constexpr float minZoom = 0.002f;
constexpr float maxZoom = 1.0f;
constexpr float maxScroll = 100.0f;

constexpr float hexHeight = 1.5f;

int ceilInt(float);

struct Tilemap;

enum Terrain : uchar {
    None,
    Ocean,
    Beach,
    Grass,
    Mountain
};

Tilemap *createTilemap();
Terrain getTile(Tilemap *map, ivec2 position);

namespace graphics {
    GLFWwindow *init(const char *title);
    void render(GLFWwindow *window, Tilemap *map, float zoom);

    void moveCamera(Direction direction, float distance);
}
