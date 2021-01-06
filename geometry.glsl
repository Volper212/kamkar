R""(
#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

in vec3 vColor[];
out vec3 fColor;

layout(location = 4) uniform float zoom;
layout(location = 5) uniform vec2 positions[6];

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