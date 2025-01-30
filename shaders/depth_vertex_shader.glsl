#version 430 core

/**
 * @brief Pozycja wierzchołka w przestrzeni lokalnej.
 */
layout (location = 0) in vec3 aPos;

/**
 * @brief Macierz transformacji z przestrzeni świata do przestrzeni światła.
 * 
 * Używana do przekształcenia pozycji wierzchołka, aby uzyskać współrzędne do generowania mapy cieni.
 */
uniform mat4 lightSpaceMatrix;

/**
 * @brief Macierz modelu, transformująca wierzchołek z przestrzeni lokalnej do przestrzeni świata.
 */
uniform mat4 model;

/**
 * @brief Główna funkcja vertex shadera.
 * 
 * Przekształca pozycję wierzchołka do przestrzeni światła, co jest wykorzystywane 
 * w shadow mappingu do obliczania cieni rzutowanych przez obiekty.
 */
void main() {
    // Przekształcenie pozycji wierzchołka do przestrzeni światła
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
