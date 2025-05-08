#version 430 core

/**
 * @brief Pozycja fragmentu w przestrzeni świata.
 */
in vec3 FragPos;

/**
 * @brief Normalny wektor powierzchni w przestrzeni świata.
 */
in vec3 Normal;

/**
 * @brief Współrzędne tekstury.
 */
in vec2 TexCoord;

/**
 * @brief Pozycja fragmentu w przestrzeni światła dla wielu źródeł światła.
 */
in vec4 FragPosLightSpace[10];

/**
 * @struct Light
 * @brief Struktura reprezentująca pojedyncze źródło światła.
 */
struct Light {
    vec3 position;    /**< Pozycja światła w przestrzeni świata. */
    vec3 color;       /**< Kolor światła. */
    sampler2D shadowMap; /**< Mapa cieni przypisana do światła. */
};

/**
 * @brief Liczba aktywnych źródeł światła.
 */
uniform int numLights;

/**
 * @brief Tablica świateł dostępnych w scenie.
 */
uniform Light lights[10];

/**
 * @brief Pozycja widza/kamery w przestrzeni świata.
 */
uniform vec3 viewPos;

/**
 * @brief Tekstura używana do rysowania obiektu.
 */
uniform sampler2D texture1;

/**
 * @brief Siła wpływu cienia na oświetlenie (wartość domyślna 1.5).
 */
uniform float shadowStrength = 1.0;

/**
 * @brief Tryb debugowania (0 = wyłączony, wartości >0 wskazują konkretne źródło światła).
 */
uniform int debugMode = 0;

/**
 * @brief Kolor wyjściowy piksela.
 */
out vec4 FragColor;

/**
 * @brief Oblicza dynamiczne przesunięcie (bias) w celu redukcji artefaktów cieniowania.
 *
 * @param normal Wektor normalny powierzchni.
 * @param lightDir Kierunek do źródła światła.
 * @return Wartość przesunięcia (bias).
 */
float computeBias(vec3 normal, vec3 lightDir) {
    return max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
}

/**
 * @brief Oblicza wartość cienia dla fragmentu.
 *
 * @param fragPosLight Pozycja fragmentu w przestrzeni światła.
 * @param shadowMap Mapa cieni przypisana do światła.
 * @param normal Wektor normalny powierzchni.
 * @param lightDir Kierunek do źródła światła.
 * @return Wartość cienia (1.0 = całkowicie zacienione, 0.0 = bez cienia).
 */
float ShadowCalculation(vec4 fragPosLight, sampler2D shadowMap, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;  // Przekształcenie współrzędnych do przestrzeni NDC
    projCoords = projCoords * 0.5 + 0.5; // Przekształcenie do przedziału [0,1]

    // Jeśli fragment znajduje się poza zakresem mapy cieni, nie rzutuje cienia
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0 || projCoords.z > 1.0)
        return 0.0; 

    float closestDepth = texture(shadowMap, projCoords.xy).r; // Pobranie głębokości zapisanej w mapie cieni
    float dynamicBias = computeBias(normal, lightDir);

    // Jeśli aktualna głębokość jest większa niż zapisana w mapie + bias, piksel jest w cieniu
    return projCoords.z > closestDepth + dynamicBias ? 1.0 : 0.0;
}

/**
 * @brief Główna funkcja fragment shadera.
 */
void main() {
    vec3 color = texture(texture1, TexCoord).rgb; // Pobranie koloru z tekstury
    vec3 normal = normalize(Normal); // Normalizacja wektora normalnego
    vec3 viewDir = normalize(viewPos - FragPos); // Kierunek do widza/kamery
    vec3 result = vec3(0.0); // Inicjalizacja wyniku końcowego

    for (int i = 0; i < numLights; ++i) {
        vec3 lightDir = normalize(lights[i].position - FragPos); // Kierunek do światła
        float distance = length(lights[i].position - FragPos); // Odległość od światła
        float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.02 * (distance * distance)); // Współczynnik osłabienia

        // Składowa ambient (otoczenia)
        vec3 ambient = 0.2 * lights[i].color * color;

        // Składowa diffuse (rozproszonego światła)
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * color;

        // Składowa specular (odbicia)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        vec3 specular = vec3(0.3) * spec * lights[i].color;

        // Obliczenie wartości cienia
        float shadow = ShadowCalculation(FragPosLightSpace[i], lights[i].shadowMap, normal, lightDir);
        shadow = clamp(shadow, 0.0, 1.0); // Ograniczenie wartości do przedziału [0,1]

        // Tryb debugowania: jeśli wybrano konkretne światło, zwróć wartość cienia
        if (debugMode == i+1) { 
            FragColor = vec4(vec3(shadow), 1.0); 
            return; 
        }

        // Sumowanie składowych z uwzględnieniem osłabienia i wpływu cieni
        result += (ambient + (1.0 - shadow * shadowStrength) * (diffuse + specular)) * attenuation;
    }

    // Ustawienie koloru piksela
    FragColor = vec4(result, 1.0);
}
