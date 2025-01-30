#ifndef WALL_H
#define WALL_H

#include <vector>
#include <array>
#include "ShapeObject.h"

/**
 * @class Wall
 * @brief Klasa reprezentująca ścianę jako obiekt 3D.
 *
 * Klasa `Wall` dziedziczy po `DrawableObject` i `TransformableObject`,
 * co umożliwia renderowanie oraz transformacje w przestrzeni 3D.
 * Obsługuje operacje takie jak translacja, rotacja, skalowanie oraz
 * przypisywanie tekstury.
 */
class Wall : public DrawableObject, public TransformableObject {
public:
    /**
     * @brief Identyfikator VAO (Vertex Array Object) OpenGL.
     */
    GLuint vao = 0;

    /**
     * @brief Identyfikator VBO (Vertex Buffer Object) OpenGL.
     */
    GLuint vbo = 0;

    /**
     * @brief Identyfikator EBO (Element Buffer Object) OpenGL.
     */
    GLuint ebo = 0;

    /**
     * @brief Konstruktor tworzący ścianę o określonych wymiarach i pozycji.
     *
     * @param width Szerokość ściany.
     * @param height Wysokość ściany.
     * @param x Współrzędna X pozycji środka ściany.
     * @param y Współrzędna Y pozycji środka ściany.
     * @param z Współrzędna Z pozycji środka ściany.
     * @param texture Identyfikator tekstury OpenGL przypisanej do ściany.
     */
    Wall(float width, float height, float x, float y, float z, GLuint texture);

    /**
     * @brief Konfiguruje bufory wierzchołków i indeksów dla OpenGL.
     *
     * Metoda przygotowuje dane wierzchołkowe, współrzędne tekstur oraz indeksy
     * dla poprawnego renderowania ściany.
     */
    void setupBuffers();

    /**
     * @brief Rysuje ścianę przy użyciu podanego programu cieniującego i macierzy transformacji.
     *
     * @param shaderProgram Identyfikator programu cieniującego OpenGL.
     * @param model Macierz modelu, określająca transformację obiektu w przestrzeni świata.
     * @param view Macierz widoku, określająca pozycję kamery i jej orientację.
     * @param projection Macierz projekcji, definiująca sposób odwzorowania 3D na 2D.
     */
    void draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) override;

    /**
     * @brief Przesuwa ścianę o podany wektor kierunku.
     *
     * @param direction Wektor przesunięcia (x, y, z).
     */
    void translate(const glm::vec3& direction) override;

    /**
     * @brief Obraca ścianę o zadany kąt wokół podanej osi.
     *
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     */
    void rotate(float angle, const glm::vec3& axis) override;

    /**
     * @brief Obraca ścianę wokół osi przechodzącej przez dany punkt.
     *
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     * @param point Punkt, wokół którego wykonywany jest obrót.
     */
    void rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) override;

    /**
     * @brief Skaluje ścianę o podane współczynniki.
     *
     * @param sx Współczynnik skalowania w osi X.
     * @param sy Współczynnik skalowania w osi Y.
     */
    void scale(float sx, float sy) override;

    /**
     * @brief Obraca ścianę wokół własnej osi.
     *
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     */
    void rotateAround(float angle, const glm::vec3& axis);

private:
    /**
     * @brief Wektor przechowujący współrzędne wierzchołków ściany.
     */
    std::vector<float> vertices;

    /**
     * @brief Wektor przechowujący współrzędne tekstur dla wierzchołków.
     */
    std::vector<float> texCoords;

    /**
     * @brief Wektor przechowujący indeksy wierzchołków do renderowania za pomocą EBO.
     */
    std::vector<unsigned int> indices;

    /**
     * @brief Identyfikator tekstury przypisanej do ściany.
     */
    GLuint textureID;

    /**
     * @brief Pozycja ściany w przestrzeni 3D.
     */
    glm::vec3 position;

    /**
     * @brief Rozmiar ściany (szerokość, wysokość).
     */
    glm::vec2 size;
};

#endif // WALL_H
