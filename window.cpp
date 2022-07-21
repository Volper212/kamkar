module;
export module window;
export import GLFW;
import vec2;

vec2<float> getCursorPosition(GLFWwindow* window) {
    vec2<double> cursor;
    glfwGetCursorPos(window, &cursor.x, &cursor.y);
    return vec2<float>(cursor);
}

constexpr float sqrt3 = 1.73205080757f;

vec2<float> convertWindowToAxialCoordinates(GLFWwindow* window, vec2<float> position) {
    vec2<int> size;
    glfwGetWindowSize(window, &size.x, &size.y);
    vec2<float> const scaled =
        (position / (size * 0.5f) - 1) / vec2<float>(sqrt3 * size.y / size.x, -1.5f);
    return vec2<float>(scaled.x - scaled.y / 2, scaled.y);
}

export template <class Events>
struct Window {
    Window(char const* title, Events* events) {
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

        glfwSetWindowUserPointer(window, events);
        glfwSetScrollCallback(window, handleScroll);
        glfwSetMouseButtonCallback(window, handleClick);
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

    static Events* getEvents(GLFWwindow* window) {
        return static_cast<Events*>(glfwGetWindowUserPointer(window));
    }

    static void handleScroll(GLFWwindow* window, double xOffset, double yOffset) {
        getEvents(window)->handleScroll(yOffset);
    }

    static void handleClick(GLFWwindow* window, int button, int action, int mods) {
        if (action != GLFW_PRESS) return;
        if (button != GLFW_MOUSE_BUTTON_LEFT && button != GLFW_MOUSE_BUTTON_RIGHT) return;
        getEvents(window)->handleClick(
            convertWindowToAxialCoordinates(window, getCursorPosition(window)),
            button == GLFW_MOUSE_BUTTON_LEFT);
    }
};
