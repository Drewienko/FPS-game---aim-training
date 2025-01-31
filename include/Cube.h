#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <array>
#include "ShapeObject.h"

#include <iostream>

/**
 * @class Cube
 * @brief Klasa reprezentująca sześcian jako obiekt 3D.
 *
 * Klasa Cube dziedziczy po ShapeObject, dzięki czemu
 * obsługuje zarówno renderowanie, jak i transformacje w przestrzeni 3D.
 * Pozwala na manipulację położeniem, rotacją, skalowaniem oraz przypisywanie tekstur do ścian.
 */
class Cube : public ShapeObject {
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
     * @brief Konstruktor tworzący sześcian o określonym rozmiarze i kolorze.
     *
     * @param size Rozmiar sześcianu (długość krawędzi).
     * @param x Współrzędna X środka sześcianu.
     * @param y Współrzędna Y środka sześcianu.
     * @param z Współrzędna Z środka sześcianu.
     * @param texture Identyfikator tekstury OpenGL przypisanej do każdej z 6 ścian.
     */
    Cube(float size, float x, float y, float z, GLuint texture);

    /**
     * @brief Konfiguruje bufory wierzchołków i indeksów dla OpenGL.
     *
     * Metoda przygotowuje dane wierzchołkowe, współrzędne tekstur oraz indeksy
     * dla poprawnego renderowania sześcianu.
     */
    void setupBuffers();

    /**
     * @brief Rysuje sześcian przy użyciu podanego programu cieniującego i macierzy transformacji.
     *
     * @param shaderProgram Identyfikator programu cieniującego OpenGL.
     * @param model Macierz modelu, określająca transformację obiektu w przestrzeni świata.
     * @param view Macierz widoku, określająca pozycję kamery i jej orientację.
     * @param projection Macierz projekcji, definiująca sposób odwzorowania 3D na 2D.
     */
    void draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) override;

    /**
     * @brief Przesuwa sześcian o podany wektor kierunku.
     *
     * @param direction Wektor przesunięcia (x, y, z).
     */
    void translate(const glm::vec3& direction) override;

    /**
     * @brief Obraca sześcian o zadany kąt wokół podanej osi.
     *
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     */
    void rotate(float angle, const glm::vec3& axis) override;

    /**
     * @brief Obraca sześcian wokół osi przechodzącej przez dany punkt.
     *
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     * @param point Punkt wokół którego wykonywany jest obrót.
     */
    void rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) override;

    /**
     * @brief Skaluje sześcian o podane współczynniki.
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

    /**
     * @brief Ustawia teksturę dla jednej ze ścian sześcianu.
     *
     * @param side Indeks ściany (0-5), gdzie 0 = przód, 1 = tył, 2 = lewa, 3 = prawa, 4 = góra, 5 = dół.
     * @param textureID Identyfikator tekstury OpenGL.
     */
    void setTextureForSide(int side, GLuint textureID);

private:
    /**
     * @brief Wektor przechowujący współrzędne wierzchołków sześcianu.
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
     * @brief Tablica przechowująca identyfikatory tekstur dla każdej ściany sześcianu.
     */
    std::array<GLuint, 6> textures = { 0, 0, 0, 0, 0, 0 };
};

#endif // CUBE_H
