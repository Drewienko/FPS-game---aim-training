#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D shadowMap;

void main() {
    float depth = texture(shadowMap, TexCoord).r;
    FragColor = vec4(vec3(depth), 1.0);
}
