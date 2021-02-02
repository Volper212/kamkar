R""(
#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

in vec3 vColor[];
out vec3 fColor;

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
    fColor = vColor[0];
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    vec2 position = gl_in[0].gl_Position.xy;
    for (int i = 0; i < 6; ++i) {
        gl_Position.xy = (position + positions[i]) * zoom;
        EmitVertex();
    }
    EndPrimitive();
}
)""
