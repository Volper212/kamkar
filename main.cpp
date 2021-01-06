#include <stdio.h>
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

static int roundInt(float input) {
    return ceilInt(input - 0.5f);
}

static float scroll = 50.0f;

static void handleScroll(GLFWwindow *window, double xOffset, double yOffset) {
    scroll = clamp(scroll + float(yOffset), 0.0f, maxScroll);
}

static float getZoom(float scroll) {
    static const float factor = logf(maxZoom / minZoom) / maxScroll;
    return minZoom * expf(factor * scroll);
}

constexpr float cameraSpeed = 1.0f;

int main() {
    Tilemap *tilemap = createTilemap();
    GLFWwindow *window = graphics::init("Kamkar");

    glfwSetScrollCallback(window, handleScroll);

    float previousFrame = float(glfwGetTime());
    float deltaTime = 0;
    float timeCounter = 0;
    int frameCounter = 0;

    while (!glfwWindowShouldClose(window)) {
        const float zoom = getZoom(scroll);
        graphics::render(window, tilemap, zoom);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            return 0;
        }

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

        if (timeCounter > 1.0) {
            printf("%i\n", roundInt(frameCounter / timeCounter));
            timeCounter = 0.0;
            frameCounter = 0;
        }
        timeCounter += deltaTime;
        ++frameCounter;
    }
}