#version 430 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace[10];

struct Light {
    vec3 position;
    vec3 color;
    sampler2D shadowMap;
};

uniform int numLights;
uniform Light lights[10];
uniform vec3 viewPos;
uniform sampler2D texture1;
uniform float shadowStrength = 1.0;
uniform int debugMode = 0;
out vec4 FragColor;

// Compute adaptive bias based on surface slope
float computeBias(vec3 normal, vec3 lightDir) {
    return max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
}

// Shadow calculation with adaptive bias
float ShadowCalculation(vec4 fragPosLight, sampler2D shadowMap, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5; 

    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0 || projCoords.z > 1.0)
        return 0.0; 

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float dynamicBias = computeBias(normal, lightDir);

    return projCoords.z > closestDepth + dynamicBias ? 1.0 : 0.0;
}

void main() {
    vec3 color = texture(texture1, TexCoord).rgb;
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    for (int i = 0; i < numLights; ++i) {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.02 * (distance * distance));

        vec3 ambient = 0.4 * lights[i].color * color;
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * color;

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        vec3 specular = vec3(0.3) * spec * lights[i].color;

        float shadow = ShadowCalculation(FragPosLightSpace[i], lights[i].shadowMap, normal, lightDir);
        shadow = clamp(shadow, 0.0, 1.0);

        if (debugMode == 1) { FragColor = vec4(vec3(shadow), 1.0); return; }
        if (debugMode == 2) { FragColor = vec4(vec3(computeBias(normal, lightDir)), 1.0); return; }

        result += (ambient + (1.0 - shadow * shadowStrength) * (diffuse + specular)) * attenuation;
    }

    FragColor = vec4(result, 1.0);
}
