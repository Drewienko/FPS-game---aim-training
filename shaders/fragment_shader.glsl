#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace[10]; // Light-space positions for each light

struct Light {
    vec3 position;
    vec3 color;
    sampler2D shadowMap;
};

uniform int numLights; // Number of active lights
uniform Light lights[10];
uniform vec3 viewPos;
uniform sampler2D texture1; // Per-face texture

out vec4 FragColor;

// Shadow calculation for a single light
float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // Perspective divide
    projCoords = projCoords * 0.5 + 0.5; // Transform to [0, 1] range

    // Ignore fragments outside shadow map bounds
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0 || projCoords.z > 1.0) {
        return 0.0; // Not in shadow
    }

    // Sample closest depth from shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Add bias to prevent shadow acne
    //float bias = max(0.005 * (1.0 - dot(normalize(Normal), normalize(FragPos - fragPosLightSpace.xyz))), 0.001);
    float bias = 0.05;
    // Check if fragment is in shadow
    return currentDepth > closestDepth + bias ? 1.0 : 0.0;
}

void main() {
    vec3 color = texture(texture1, TexCoord).rgb; // Sample the texture
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    for (int i = 0; i < numLights; ++i) {
    vec3 lightDir = normalize(lights[i].position - FragPos);
    float distance = length(lights[i].position - FragPos);

    // Light attenuation
    float attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * (distance * distance));

    // Ambient light
    vec3 ambient = 0.2 * lights[i].color * color;

    // Diffuse light
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lights[i].color * color;

    // Specular light
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    vec3 specular = vec3(0.3) * spec * lights[i].color;

    // Shadow calculation
    float shadow = ShadowCalculation(FragPosLightSpace[i], lights[i].shadowMap);

    // Apply shadows: Reduce diffuse and specular light in shadowed areas
    result += (ambient + (1.0 - shadow) * (diffuse + specular)) * attenuation;
}

    FragColor = vec4(result, 1.0);

}
