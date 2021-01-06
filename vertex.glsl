R""(
#version 430 core

in uint color;
out vec3 vColor;

layout(location = 0) uniform vec2 hexSize;
layout(location = 1) uniform int width;
layout(location = 2) uniform ivec2 offset;
layout(location = 3) uniform vec2 camera;

const vec3 colors[] = vec3[](
    vec3(0.96, 0.96, 0.86),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(1.0, 1.0, 1.0)
);

void main() {
    int x = gl_VertexID % width;
    int y = gl_VertexID / width;
    gl_Position = vec4(
        (vec2(
            x + y % 2 * 0.5,
            y
        ) + offset + camera) * hexSize,
        0.0, 1.0);
    vColor = colors[color];
}
)""