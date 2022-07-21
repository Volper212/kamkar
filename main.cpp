#include "kamkar.h"

#if defined(WIN32) && defined(NDEBUG)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

static float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static vec2<int>* selectedEntity = nullptr;

static vec2<int> entities[] = {
    { 0, 0 },
    { 0, 1 },
};

static float getZoom(float scroll) {
    static const float factor = logf(maxZoom / minZoom) / maxScroll;
    return minZoom * expf(factor * scroll);
}
struct App {
    float scroll = 50.0f;

    void handleScroll(float offset) {
        scroll = clamp(scroll + offset, 0.0f, maxScroll);
    }

    void handleClick(vec2<float> cursor, bool isLeft) {
        const vec2<float> camera = graphics::getCamera();
        const float zoom = getZoom(scroll);
        const float q = cursor.x / zoom - camera.x + camera.y / 2;
        const float r = cursor.y / zoom - camera.y;
        int rx = iround(q);
        const int ry = iround(-q - r);
        int rz = iround(r);
        const float dx = fabsf(rx - q);
        const float dy = fabsf(ry + q + r);
        const float dz = fabsf(rz - r);
        if (dx > dy && dx > dz) {
            rx = -ry - rz;
        } else {
            if (dy <= dz) { rz = -rx - ry; }
        }
        const vec2<int> result = vec2<int>{ rx, rz } + graphics::getCameraHex();
        if (isLeft) {
            for (vec2<int>& entity : entities) {
                if (entity == result) {
                    selectedEntity = &entity;
                    return;
                }
            }
            selectedEntity = nullptr;
        } else {
            if (selectedEntity == nullptr) return;
            *selectedEntity = result;
        }
    }
};

int main() {
    Tilemap* tilemap = createTilemap();

    App app;
    Window<App> w("Kamkar", &app);
    GLFWwindow* window = w.get();
    graphics::init(w.getSize());

    float previousFrame = float(glfwGetTime());
    float deltaTime = 0;

    while (!glfwWindowShouldClose(window)) {
        constexpr int radius = 10;
        for (vec2<int> entity : entities) {
            for (int x = -radius; x <= radius; ++x) {
                for (int y = -radius - min(x, 0); y <= radius - max(x, 0); ++y) {
                    getTile(tilemap, entity + vec2<int>{ x, y })->isVisible = true;
                }
            }
        }

        const float zoom = getZoom(app.scroll);
        graphics::render(window, tilemap, entities, lengthof(entities), zoom);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) return 0;

        constexpr float cameraSpeed = 1.0f;
        const float distance = deltaTime * cameraSpeed / zoom;

        if (glfwGetKey(window, GLFW_KEY_DOWN)) graphics::moveCamera(Y, distance);
        if (glfwGetKey(window, GLFW_KEY_UP)) graphics::moveCamera(Y, -distance);
        if (glfwGetKey(window, GLFW_KEY_LEFT)) graphics::moveCamera(X, distance);
        if (glfwGetKey(window, GLFW_KEY_RIGHT)) graphics::moveCamera(X, -distance);

        const float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - previousFrame;
        previousFrame = currentFrame;
    }
}
