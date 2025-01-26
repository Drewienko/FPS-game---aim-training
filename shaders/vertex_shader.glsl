#version 460 core
layout (location = 0) in vec3 aPos;       // Vertex position
layout (location = 1) in vec2 aTexCoord; // Texture coordinates
layout (location = 2) in vec3 aNormal;   // Normal vector

out vec2 TexCoord;   // Pass texture coordinates to the fragment shader
out vec3 FragPos;    // Pass world-space position to the fragment shader
out vec3 Normal;     // Pass world-space normal to the fragment shader

uniform mat4 model;       // Model matrix
uniform mat4 view;        // View matrix
uniform mat4 projection;  // Projection matrix

void main() {
    // Transform vertex position into world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform the normal into world space
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pass texture coordinates
    TexCoord = aTexCoord;

    // Transform vertex position into clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
