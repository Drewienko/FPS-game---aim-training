#ifndef TRANSFORMABLEOBJECT_H
#define TRANSFORMABLEOBJECT_H

#include "GameObject.h"
#include <glm/vec3.hpp>

/**
 * @class TransformableObject
 * @brief Klasa bazowa dla obiektów, które mogą być transformowane (translacja, rotacja, skalowanie).
 */
class TransformableObject : virtual public GameObject
{
public:
    /**
     * @brief Wirtualny destruktor klasy TransformableObject.
     */
    virtual ~TransformableObject() = default;

    /**
     * @brief Przesuwa obiekt o podany wektor kierunku.
     * @param direction Wektor przesunięcia (x, y, z).
     */
    virtual void translate(const glm::vec3& direction) = 0;

    /**
     * @brief Obraca obiekt o zadany kąt wokół podanej osi.
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     */
    virtual void rotate(float angle, const glm::vec3& axis) = 0;

    /**
     * @brief Obraca obiekt o zadany kąt wokół osi przechodzącej przez dany punkt.
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     * @param point Punkt wokół którego wykonywany jest obrót.
     */
    virtual void rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) = 0;

    /**
     * @brief Skaluje obiekt o podane współczynniki w osiach X i Y.
     * @param sx Współczynnik skalowania w osi X.
     * @param sy Współczynnik skalowania w osi Y.
     */
    virtual void scale(float sx, float sy) = 0;
};

#endif // TRANSFORMABLEOBJECT_H
