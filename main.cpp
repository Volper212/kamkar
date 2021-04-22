#include "kamkar.h"

#if defined(WIN32) && defined(NDEBUG)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

static float clamp(float value, float min, float max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

static float scroll = 50.0f;

static void handleScroll(GLFWwindow *window, double xOffset, double yOffset) {
    scroll = clamp(scroll + float(yOffset), 0.0f, maxScroll);
}

static vec2<int> entities[] = {
    0, 0,
    0, 1,
};

static vec2<int> *selectedEntity = nullptr;

static float getZoom(float scroll) {
    static const float factor = logf(maxZoom / minZoom) / maxScroll;
    return minZoom * expf(factor * scroll);
}

static void handleClick(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        vec2<float> cursor = { float(x), float(y) };
        vec2<int> windowSize;
        glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
        cursor.x = cursor.x / (windowSize.x / 2.0f) - 1.0f;
        cursor.y = 1.0f - cursor.y / (windowSize.y / 2.0f);
        const vec2 camera = graphics::getCamera();
        const float zoom = getZoom(scroll);
        const float hexWidth = sqrt3 * windowSize.y / windowSize.x;
        const float q = (cursor.x / hexWidth - cursor.y / hexHeight / 2) / zoom - camera.x + camera.y / 2;
        const float r = (cursor.y / hexHeight) / zoom - camera.y;
        int rx = iround(q);
        const int ry = iround(-q - r);
        int rz = iround(r);
        const float dx = fabsf(rx - q);
        const float dy = fabsf(ry + q + r);
        const float dz = fabsf(rz - r);
        if (dx > dy && dx > dz) {
            rx = -ry-rz;
        } else {
            if (dy <= dz) {
                rz = -rx-ry;
            }
        }
        const vec2<int> result = vec2<int>{ rx, rz } + graphics::getCameraHex();
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                for (vec2<int> &entity : entities) {
                    if (entity == result) {
                        selectedEntity = &entity;
                        return;
                    }
                }
                selectedEntity = nullptr;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                if (selectedEntity == nullptr) return;
                *selectedEntity = result;
                break;
        }
    }
}

int main() {
    Tilemap *tilemap = createTilemap();

    GLFWwindow *window = graphics::init("Kamkar");

    glfwSetScrollCallback(window, handleScroll);
    glfwSetMouseButtonCallback(window, handleClick);

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

        const float zoom = getZoom(scroll);
        graphics::render(window, tilemap, entities, lengthof(entities), zoom);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            return 0;
        }

        constexpr float cameraSpeed = 1.0f;
        const float distance = deltaTime * cameraSpeed / zoom;

        if (glfwGetKey(window, GLFW_KEY_DOWN)) {
            graphics::moveCamera(Vertical, distance);
        }

        if (glfwGetKey(window, GLFW_KEY_UP)) {
            graphics::moveCamera(Vertical, -distance);
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT)) {
            graphics::moveCamera(Horizontal, distance);
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
            graphics::moveCamera(Horizontal, -distance);
        }

        const float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - previousFrame;
        previousFrame = currentFrame;
    }
}
