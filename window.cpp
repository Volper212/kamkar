#include <GLFW/glfw3.h>
export module window;
import vec2;

export struct Events {
    void (*onScroll)(float offset, void* data);
    void* data;
};

Events const* getEvents(GLFWwindow* window) {
    return static_cast<Events const*>(glfwGetWindowUserPointer(window));
}

void handleScroll(GLFWwindow* window, double xOffset, double yOffset) {
    Events const* const events = getEvents(window);
    events->onScroll(yOffset, events->data);
}

export struct Window {
    Window(char const* title, Events events) : events(events) {
        glfwInit();
        GLFWmonitor* const monitor = glfwGetPrimaryMonitor();
        GLFWvidmode const* const mode = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

        size = { mode->width, mode->height };
        window = glfwCreateWindow(size.x, size.y, title, monitor, nullptr);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        glfwSetWindowUserPointer(window, &this->events);
        glfwSetScrollCallback(window, handleScroll);
    };

    GLFWwindow* get() {
        return window;
    }

    vec2<int> getSize() {
        return size;
    }

  private:
    GLFWwindow* window;
    vec2<int> size;
    Events events;
};
