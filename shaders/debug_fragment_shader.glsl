#version 440 core
out vec4 FragColor;
uniform sampler2D shadowMap;

void main() {
    float depth = texture(shadowMap, gl_FragCoord.xy / vec2(512.0, 512.0)).r; // Adjust resolution
    FragColor = vec4(vec3(depth), 1.0); // Show depth as grayscale
}