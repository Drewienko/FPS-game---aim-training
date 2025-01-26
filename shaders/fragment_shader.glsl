#version 460 core

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
uniform bool debugShadows = false;
uniform int debugMode = 0;
uniform float bias = 0.01;
out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, out float closestDepth, out vec3 projCoords) {
    projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0 || projCoords.z < 0.0 || projCoords.z > 1.0) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red indicates out-of-bounds
        return 0.0;
    }

    closestDepth = texture(shadowMap, projCoords.xy).r;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float sampleDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z > sampleDepth + bias ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // Debug: Shadow Factor Visualization
    if (debugMode == 3) {
        FragColor = vec4(vec3(shadow), 1.0);
        return shadow;
    }

    return shadow;
}

void main() {
    vec3 color = texture(texture1, TexCoord).rgb;
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    float totalShadow = 0.0;

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

        float closestDepth;
        vec3 projCoords;
        float shadow = ShadowCalculation(FragPosLightSpace[i], lights[i].shadowMap, closestDepth, projCoords);
        shadow = clamp(shadow, 0.0, 1.0); // Ensure shadow values remain in range

        // Debugging Outputs
        if (debugMode == 1) {
            FragColor = vec4(vec3(texture(lights[i].shadowMap, projCoords.xy).r), 1.0); // Visualize shadow map depth
            return;
        }
        if (debugMode == 2) {
            FragColor = vec4(vec3(projCoords.z - closestDepth), 1.0); // Visualize depth difference
            return;
        }
        if (debugMode == 3) {
            FragColor = vec4(vec3(bias), 1.0); // Visualize bias
            return;
        }

        totalShadow = clamp(totalShadow + shadow * attenuation, 0.0, 1.0);

        result += (ambient + (1.0 - shadow * shadowStrength) * (diffuse + specular)) * attenuation;
    }

    totalShadow = clamp(totalShadow, 0.0, 1.0); // Prevent over-darkening
    result *= (1.0 - totalShadow * shadowStrength);

    FragColor = vec4(result, 1.0);
}
