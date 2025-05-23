#version 330 core
out vec4 FragColor;

uniform vec3 crosshairColor;
uniform float lineWidth;      // in pixels (e.g. 2.0)
uniform float gapSize;        // in pixels (e.g. 5.0)
uniform float crosshairLength; // total half-length (e.g. 10.0)
uniform vec2 resolution;

void main() {
    vec2 center = resolution * 0.5;
    vec2 delta = gl_FragCoord.xy - center;

    if (abs(delta.x) > crosshairLength || abs(delta.y) > crosshairLength) {
        discard; //
    }

    bool vertical = abs(delta.x) < lineWidth && abs(delta.y) > gapSize;
    bool horizontal = abs(delta.y) < lineWidth && abs(delta.x) > gapSize;

    if (vertical || horizontal) {
        FragColor = vec4(crosshairColor, 1.0);
    } else {
        discard;
    }
}
