#ifndef SHAPEOBJECT_H
#define SHAPEOBJECT_H

#include "DrawableObject.h"
#include "TransformableObject.h"

/**
 * @class ShapeObject
 * @brief Klasa bazowa dla obiektów, które mogą być zarówno rysowane, jak i transformowane.
 *
 * Klasa `ShapeObject` łączy funkcjonalność `DrawableObject` (renderowanie w OpenGL)
 * oraz `TransformableObject` (operacje translacji, rotacji i skalowania),
 * umożliwiając łatwe zarządzanie kształtami w scenie 3D.
 */
class ShapeObject : public DrawableObject, public TransformableObject {
public:
    /**
     * @brief Wirtualny destruktor klasy ShapeObject.
     *
     * Zapewnia poprawne zarządzanie pamięcią w klasach pochodnych.
     */
    virtual ~ShapeObject() = default;

    /**
     * @brief Rysuje obiekt przy użyciu podanego programu cieniującego i macierzy transformacji.
     *
     * @param shaderProgram Identyfikator programu cieniującego OpenGL.
     * @param model Macierz modelu, określająca transformację obiektu w przestrzeni świata.
     * @param view Macierz widoku, określająca pozycję kamery i jej orientację.
     * @param projection Macierz projekcji, definiująca sposób odwzorowania 3D na 2D.
     */
    virtual void draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) override = 0;

    /**
     * @brief Przesuwa obiekt o podany wektor kierunku.
     * @param direction Wektor przesunięcia (x, y, z).
     */
    virtual void translate(const glm::vec3& direction) override = 0;

    /**
     * @brief Obraca obiekt o zadany kąt wokół podanej osi.
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     */
    virtual void rotate(float angle, const glm::vec3& axis) override = 0;

    /**
     * @brief Obraca obiekt o zadany kąt wokół osi przechodzącej przez dany punkt.
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     * @param point Punkt wokół którego wykonywany jest obrót.
     */
    virtual void rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) override = 0;

    /**
     * @brief Skaluje obiekt o podane współczynniki w osiach X i Y.
     * @param sx Współczynnik skalowania w osi X.
     * @param sy Współczynnik skalowania w osi Y.
     */
    virtual void scale(float sx, float sy) override = 0;
};

#endif // SHAPEOBJECT_H
