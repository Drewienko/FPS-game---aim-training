#version 430 core

/**
 * @brief Pozycja wierzchołka w przestrzeni lokalnej.
 */
layout (location = 0) in vec3 aPos;

/**
 * @brief Współrzędne tekstury wierzchołka.
 */
layout (location = 1) in vec2 aTexCoord;

/**
 * @brief Wektor normalny wierzchołka.
 */
layout (location = 2) in vec3 aNormal;

/**
 * @brief Macierz modelu, transformująca wierzchołek do przestrzeni świata.
 */
uniform mat4 model;

/**
 * @brief Macierz widoku, określająca pozycję i orientację kamery.
 */
uniform mat4 view;

/**
 * @brief Macierz projekcji, używana do transformacji do przestrzeni NDC (Normalized Device Coordinates).
 */
uniform mat4 projection;

/**
 * @brief Macierze przestrzeni światła dla maksymalnie 10 źródeł światła.
 * 
 * Służą do transformacji pozycji fragmentu do przestrzeni światła, co jest używane do obliczania cieni.
 */
uniform mat4 lightSpaceMatrix[10];

/**
 * @brief Liczba aktywnych źródeł światła.
 */
uniform int numLights;

/**
 * @brief Pozycja fragmentu w przestrzeni świata.
 */
out vec3 FragPos;

/**
 * @brief Przekształcony wektor normalny w przestrzeni świata.
 */
out vec3 Normal;

/**
 * @brief Współrzędne tekstury przekazywane do fragment shadera.
 */
out vec2 TexCoord;

/**
 * @brief Pozycja fragmentu w przestrzeni światła dla każdego źródła światła.
 */
out vec4 FragPosLightSpace[10];

/**
 * @brief Główna funkcja vertex shadera.
 */
void main() {
    // Transformacja pozycji wierzchołka do przestrzeni świata
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transformacja normalnych do przestrzeni świata (prawidłowa obsługa skalowania)
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Przekazanie współrzędnych tekstury
    TexCoord = aTexCoord;

    // Transformacja pozycji fragmentu do przestrzeni światła dla każdego źródła światła
    for (int i = 0; i < numLights; ++i) {
        FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0);
    }

    // Transformacja pozycji wierzchołka do przestrzeni NDC
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
