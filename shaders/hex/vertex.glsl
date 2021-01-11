R""(
#version 430 core

in uint color;
out vec3 vColor;

layout (std140, binding = 0) uniform Uniforms {
    vec2 positions[6];
    vec2 hexSize;
    vec2 camera;
    ivec2 cameraHex;
    ivec2 offset;
    int width;
    float zoom;
};

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
            x + (y & 1) * 0.5,
            y
        ) + offset + camera) * hexSize,
        0.0, 1.0);
    vColor = colors[color];
}
)""