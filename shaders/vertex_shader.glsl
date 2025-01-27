#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix[10]; // Support for up to 10 lights
uniform int numLights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace[10]; // Light-space positions for each light

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normals properly
    TexCoord = aTexCoord;

    for (int i = 0; i < numLights; ++i) {
        FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0);
    }

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
