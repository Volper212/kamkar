#pragma once
import vec2;
import window;
#include "util.h"
#include <noise.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

template <typename T>
T *emalloc(unsigned int count) {
    T *const result = static_cast<T *>(malloc(count * sizeof(T)));
    if (result == nullptr) {
        perror("Allocation with malloc failed");
        exit(1);
    }
    return result;
}

template <typename T>
T *ecalloc(unsigned int count) {
    T *const result = static_cast<T *>(calloc(count, sizeof(T)));
    if (result == nullptr) {
        perror("Allocation with calloc failed");
        exit(1);
    }
    return result;
}

constexpr float sqrt3 = 1.73205080757f;
constexpr float minZoom = 0.005f;
constexpr float maxZoom = 1.0f;
constexpr float maxScroll = 100.0f;

constexpr float hexHeight = 1.5f;

struct Tilemap;

enum Terrain : uchar {
    None,
    Ocean,
    Beach,
    Grass,
    Mountain
};

struct Tile {
    Terrain terrain : 7;
    bool isVisible : 1;
};

Tilemap *createTilemap();
Tile *getTile(Tilemap *map, vec2<int> position);

namespace graphics {
    void init(vec2<int> windowSize);
    void render(GLFWwindow *window, Tilemap *map, const vec2<int> *entities, int entityCount, float zoom);

    void moveCamera(Dimension, float distance);
    vec2<float> getCamera();
    vec2<int> getCameraHex();
}
