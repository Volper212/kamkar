#define GLEW_STATIC
#include <stdio.h>
#include <glew.h>
#include "kamkar.h"

#pragma warning(disable : 26812)
#pragma warning(disable : 33011)

template <typename T, int N>
constexpr int lengthof(T(&array)[N]) {
    return N;
}

static void __stdcall handleGLError(GLenum source, GLenum type, uint id, GLenum severity, int length, const char *message, const void *userParam) {
    puts(message);
}

enum Uniform : uchar {
    HexSize,
    Width,
    Offset,
    Camera,
    Zoom,
    Positions
};

static float hexWidth;

static uint compileShader(uint type, const char *source) {
    const uint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    #ifdef _DEBUG
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result) {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *message = allocate<char>(length);
        glGetShaderInfoLog(shader, length, &length, message);
        printf("Failed to compile %s shader: %s", type == GL_VERTEX_SHADER ? "vertex" : "fragment", message);
        free(message);
        glDeleteShader(shader);
    }
    #endif
    return shader;
}

struct Rect {
    ivec2 position;
    ivec2 size;
};

int ceilInt(float input) {
    const int trunced = int(input);
    return trunced + (trunced < input);
}

static int ceilHalf(int input) {
    return (input >> 1) + (input & 1);
}

static Rect getRect(float zoom) {
    const int tempWidth = ceilInt(2 / (zoom * hexWidth));
    const int tempHeight = ceilInt(2 / (zoom * hexHeight));
    Rect result;
    result.position.x = -ceilHalf(tempWidth) - 1;
    result.position.y = -(ceilHalf(tempHeight) + (ceilHalf(tempHeight) & 1));
    result.size.x = tempWidth + 3;
    result.size.y = tempHeight + 3;
    return result;
}

static int getArea(float zoom) {
    const ivec2 size = getRect(zoom).size;
    return size.x * size.y;
}

static ivec2 cameraHex = { 0, 0 };

static void updateHexes(Terrain *hexes, Rect rect, Tilemap *tilemap) {
    for (int x = 0; x < rect.size.x; ++x) {
        for (int y = 0; y < rect.size.y; ++y) {
            const ivec2 currentPosition = { x, y };
            ivec2 axialPosition = currentPosition + rect.position + cameraHex;
            axialPosition.x -= ceilHalf(axialPosition.y - ((cameraHex.y & 1) ^ (axialPosition.y & 1)));
            hexes[x + y * rect.size.x] = getTile(tilemap, axialPosition);
        }
    }

    glBufferData(GL_ARRAY_BUFFER, rect.size.x * rect.size.y, hexes, GL_DYNAMIC_DRAW);
}

static Terrain *hexes;

GLFWwindow *graphics::init(const char *title) {
    glfwInit();
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

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

    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, title, monitor, nullptr);
    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);
    glViewport(0, 0, mode->width, mode->height);
    #ifdef _DEBUG
    glDebugMessageCallback(handleGLError, nullptr);
    #endif

    hexWidth = sqrt3 * mode->height / mode->width;
    hexes = allocate<Terrain>(getArea(minZoom));

    uint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    float positions[] = {
        0.0f, -1.0f,
        -0.5f, -0.5f,
        0.5f, -0.5f,
        -0.5f, 0.5f,
        0.5f, 0.5f,
        0.0f, 1.0f
    };

    for (int i = 0; i < lengthof(positions); i += 2) {
        positions[i] *= hexWidth;
    }

    uint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_BYTE, 0, 0);

    uint program = glCreateProgram();
    uint vs = compileShader(GL_VERTEX_SHADER,
#include "vertex.glsl"
    );
    uint fs = compileShader(GL_FRAGMENT_SHADER,
#include "fragment.glsl"
    );
    uint gs = compileShader(GL_GEOMETRY_SHADER,
#include "geometry.glsl"
    );
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glAttachShader(program, gs);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteShader(gs);
    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDetachShader(program, gs);
    glUseProgram(program);
    glUniform2f(HexSize, hexWidth, hexHeight);
    glUniform2fv(Positions, lengthof(positions), positions);

    return window;
}

static float scrollCache = -1.0f;
static vec2 camera = { 0.0f, 0.0f };

static bool cameraChanged = true;
static bool cameraHexChanged = true;

void graphics::render(GLFWwindow *window, Tilemap *tilemap, float zoom) {
    if (cameraChanged) {
        glUniform2f(Camera, camera.x, camera.y);
    }

    if (zoom != scrollCache || cameraHexChanged) {
        const Rect rect = getRect(zoom);
        updateHexes(hexes, rect, tilemap);
        if (zoom != scrollCache) {
            glUniform1f(Zoom, zoom);
            glUniform1i(Width, rect.size.x);
            glUniform2i(Offset, rect.position.x, rect.position.y);
        }
    }
    scrollCache = zoom;
    cameraChanged = false;
    cameraHexChanged = false;
    
    #ifdef _DEBUG
    glClear(GL_COLOR_BUFFER_BIT);
    #endif
    glDrawArrays(GL_POINTS, 0, getArea(zoom));
    glfwSwapBuffers(window);
}

static int signFromBool(bool isNegative) {
    return isNegative ? -1 : 1;
}

void graphics::moveCamera(Direction direction, float distance) {
    const int sign = signFromBool(distance < 0.0f);
    camera.xy[direction] += distance;
    while (sign * camera.xy[direction] > 0.5f) {
        camera.xy[direction] -= sign;
        cameraHex.xy[direction] -= sign;
        if (direction == Vertical) {
            camera.x += signFromBool(cameraHex.y & 1) * 0.5f;
        }
        cameraHexChanged = true;
    }
    cameraChanged = true;
}