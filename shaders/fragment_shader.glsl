#version 460 core
out vec4 FragColor;

in vec2 TexCoord;   // Texture coordinates
in vec3 FragPos;    // World-space position
in vec3 Normal;     // World-space normal

uniform sampler2D texture1; // Texture sampler
uniform vec3 lightPos;      // Light position
uniform vec3 viewPos;       // Camera (viewer) position

void main() {
    // Texture color
    vec3 texColor = texture(texture1, TexCoord).rgb;

    // Ambient lighting
    vec3 ambient = 0.1 * texColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal); // Ensure normal is unit length
    vec3 lightDir = normalize(lightPos - FragPos); // Direction to light
    float diff = max(dot(norm, lightDir), 0.0); // Lambertian reflection
    vec3 diffuse = diff * texColor;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos); // Direction to camera
    vec3 reflectDir = reflect(-lightDir, norm); // Reflection direction
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // Shininess
    vec3 specular = vec3(0.5) * spec; // Specular intensity

    // Combine results
    vec3 lighting = ambient + diffuse + specular;
    FragColor = vec4(lighting, 1.0);
}
