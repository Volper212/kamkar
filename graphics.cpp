#define GLEW_STATIC
#include <glew.h>
#include <stb_image.h>
#include "kamkar.h"

#pragma warning(disable : 26812)
#pragma warning(disable : 33011)

static void
#ifdef WIN32
__stdcall
#endif
handleGLError(GLenum source, GLenum type, uint id, GLenum severity, int length, const char *message, const void *userParam) {
    puts(message);
}

static uint compileShader(uint type, const char *source) {
    const uint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

struct Rect {
    vec2<int> position;
    vec2<int> size;
};

static int ceilHalf(int input) {
    return (input >> 1) + (input & 1);
}

static uint buffers[3];
static uint hexProgram, entityProgram;
static uint vertexArrays[2];
static Terrain *hexes;
static struct {
    vec2<float> positions[12];
    vec2<float> hexSize;
    vec2<float> camera;
    vec2<int> cameraHex;
    vec2<int> offset;
    int width;
    float zoom;
} uniforms;

static Rect getRect(float zoom) {
    const int tempWidth = iceil(2 / (zoom * uniforms.hexSize.x));
    const int tempHeight = iceil(2 / (zoom * hexHeight));
    Rect result;
    result.position.x = -ceilHalf(tempWidth) - 1;
    result.position.y = -(ceilHalf(tempHeight) + (ceilHalf(tempHeight) & 1));
    result.size.x = tempWidth + 3;
    result.size.y = tempHeight + 3;
    return result;
}

static int getArea(float zoom) {
    const vec2<int> size = getRect(zoom).size;
    return size.x * size.y;
}

vec2<float> graphics::getCamera() {
    return uniforms.camera;
}

vec2<int> graphics::getCameraHex() {
    vec2<int> result = uniforms.cameraHex;
    result.x -= ceilHalf(result.y);
    return result;
}

void graphics::init(vec2<int> size) {
    glewInit();
    glViewport(0, 0, size.x, size.y);
    #ifdef _DEBUG
    glDebugMessageCallback(handleGLError, nullptr);
    #endif

    uniforms.hexSize.x = sqrt3 * size.y / size.x;
    uniforms.hexSize.y = hexHeight;
    hexes = emalloc<Terrain>(getArea(minZoom));

    vec2<float> positions[] = {
        0.0f, -1.0f,
        -0.5f, -0.5f,
        0.5f, -0.5f,
        -0.5f, 0.5f,
        0.5f, 0.5f,
        0.0f, 1.0f
    };

    for (int i = 0; i < lengthof(positions); ++i) {
        positions[i].x *= uniforms.hexSize.x;
    }

    for (int i = 0; i < lengthof(positions); ++i) {
        uniforms.positions[i * 2] = positions[i];
    }
    
    glGenVertexArrays(lengthof(vertexArrays), vertexArrays);
    glGenBuffers(lengthof(buffers), buffers);
    glBindBuffer(GL_UNIFORM_BUFFER, buffers[0]);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, buffers[0]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms), &uniforms, GL_DYNAMIC_DRAW);
    glBindVertexArray(vertexArrays[0]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_BYTE, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, getArea(minZoom), nullptr, GL_DYNAMIC_DRAW);
    glBindVertexArray(vertexArrays[1]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(0, 2, GL_INT, 0, 0);

    {
        const uint program = hexProgram = glCreateProgram();
        const uint vs = compileShader(GL_VERTEX_SHADER,
            #include "shaders/hex/vertex.glsl"
        );
        const uint fs = compileShader(GL_FRAGMENT_SHADER,
            #include "shaders/hex/fragment.glsl"
        );
        const uint gs = compileShader(GL_GEOMETRY_SHADER,
            #include "shaders/hex/geometry.glsl"
        );
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glAttachShader(program, gs);
        glLinkProgram(program);
        #ifdef _DEBUG
        glValidateProgram(program);
        #endif
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteShader(gs);
        glDetachShader(program, vs);
        glDetachShader(program, fs);
        glDetachShader(program, gs);
    }
    {
        const uint program = entityProgram = glCreateProgram();
        const uint vs = compileShader(GL_VERTEX_SHADER,
            #include "shaders/entity/vertex.glsl"
        );
        const uint fs = compileShader(GL_FRAGMENT_SHADER, 
            #include "shaders/entity/fragment.glsl"
        );
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        #ifdef _DEBUG
        glValidateProgram(program);
        #endif
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDetachShader(program, vs);
        glDetachShader(program, fs);
    }
    
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int width, height, nrChannels;
    uchar *data = stbi_load("textures/player.bmp", &width, &height, &nrChannels, 0);

    uint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

void graphics::render(GLFWwindow *window, Tilemap *tilemap, const vec2<int> *entities, int entityCount, float zoom) {
    glUseProgram(hexProgram);
    glBindVertexArray(vertexArrays[0]);

    const Rect rect = getRect(zoom);

    uniforms.zoom = zoom;
    uniforms.width = rect.size.x;
    uniforms.offset = rect.position;
    for (int x = 0; x < rect.size.x; ++x) {
        for (int y = 0; y < rect.size.y; ++y) {
            const vec2<int> currentPosition = { x, y };
            vec2<int> axialPosition = currentPosition + rect.position + uniforms.cameraHex;
            axialPosition.x -= ceilHalf(axialPosition.y - ((uniforms.cameraHex.y & 1) ^ (axialPosition.y & 1)));
            const Tile *tile = getTile(tilemap, axialPosition);
            hexes[x + y * rect.size.x] = tile->isVisible ? tile->terrain : Terrain::None;
        }
    }

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(uniforms), &uniforms);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, rect.size.x * rect.size.y, hexes);
    
    #ifdef _DEBUG
    glClear(GL_COLOR_BUFFER_BIT);
    #endif

    glDrawArrays(GL_POINTS, 0, getArea(zoom));

    glUseProgram(entityProgram);
    glBindVertexArray(vertexArrays[1]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, entityCount * sizeof(vec2<float>), entities, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_POINTS, 0, entityCount);

    glfwSwapBuffers(window);
}

static int signFromBool(bool isNegative) {
    return isNegative ? -1 : 1;
}

void graphics::moveCamera(Direction direction, float distance) {
    const int sign = signFromBool(distance < 0.0f);
    uniforms.camera.xy[direction] += distance;
    while (sign * uniforms.camera.xy[direction] > 0.5f) {
        uniforms.camera.xy[direction] -= sign;
        uniforms.cameraHex.xy[direction] -= sign;
        if (direction == Vertical) {
            uniforms.camera.x += signFromBool(uniforms.cameraHex.y & 1) * 0.5f;
        }
    }
}
