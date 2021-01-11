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

static ivec2 entities[] = {
    0, 0,
};

static float getZoom(float scroll) {
    static const float factor = logf(maxZoom / minZoom) / maxScroll;
    return minZoom * expf(factor * scroll);
}

static void handleClick(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        vec2 cursor = { float(x), float(y) };
        ivec2 windowSize;
        glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
        cursor.x = cursor.x / (windowSize.x / 2) - 1.0f;
        cursor.y = 1.0f - cursor.y / (windowSize.y / 2);
        const float zoom = getZoom(scroll);
        float hexWidth = sqrt3 * windowSize.y / windowSize.x;
        float q = (cursor.x / hexWidth - cursor.y / hexHeight / 2) / zoom;
        float r = (cursor.y / hexHeight) / zoom;
        vec2 camera = graphics::getCamera();
        q -= camera.x * hexWidth;
        r -= camera.y * hexHeight;
        int rx = iround(q);
        int ry = iround(-q - r);
        int rz = iround(r);
        float dx = fabsf(rx - q);
        float dy = fabsf(ry + q + r);
        float dz = fabsf(rz - r);
        if (dx > dy && dx > dz) {
            rx = -ry-rz;
        } else {
            if (dy <= dz) {
                rz = -rx-ry;
            }
        }
        ivec2 result = ivec2{ rx, rz } + graphics::getCameraHex();
        entities[0] = result;
    }
}

int main() {
    Tilemap *tilemap = createTilemap();

    constexpr int radius = 10;
    for (ivec2 entity : entities) {
        for (int x = -radius; x <= radius; ++x) {
            for (int y = -radius - min(x, 0); y <= radius - max(x, 0); ++y) {
                getTile(tilemap, entity + ivec2{ x, y })->isVisible = true;
            }
        }
    }

    GLFWwindow *window = graphics::init("Kamkar");

    glfwSetScrollCallback(window, handleScroll);
    glfwSetMouseButtonCallback(window, handleClick);

    float previousFrame = float(glfwGetTime());
    float deltaTime = 0;

    while (!glfwWindowShouldClose(window)) {
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
