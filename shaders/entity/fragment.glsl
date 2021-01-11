R""(
#version 430 core

out vec4 color;

uniform sampler2D textureSampler;

void main() {
    color = texture2D(textureSampler, gl_PointCoord);
    //color = vec3(0.96, 0.96, 0.86);
}
)""