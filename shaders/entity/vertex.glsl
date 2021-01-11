R""(
#version 430 core

in ivec2 position;

layout (std140, binding = 0) uniform Uniforms {
    vec2 positions[6];
    vec2 hexSize;
    vec2 camera;
    ivec2 cameraHex;
    ivec2 offset;
    int width;
    float zoom;
};

void main() {
    float x = position.x;
    int adjustment = (cameraHex.y & 1) + (position.y & 1);
    int temp = position.y - adjustment;
    x += (temp >> 1) + (temp & 1) + adjustment * 0.5;
    gl_Position = vec4(
        (vec2(x, position.y) - cameraHex + camera) * hexSize * zoom,
        0.0, 1.0);
    gl_PointSize = 700.0 * zoom;
}
)""