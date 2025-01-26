#ifndef DRAWABLEOBJECT_H
#define DRAWABLEOBJECT_H

#include "GameObject.h"
#include <GL/glew.h>
#include "PrimitiveRenderer.h"
#include <cmath>

/**
 * @class DrawableObject
 * @brief Klasa bazowa dla obiektów, które mogą być rysowane.
 */
class DrawableObject : virtual public GameObject
{
public:
    /**
     * @brief Wirtualny destruktor klasy DrawableObject.
     */
    virtual ~DrawableObject() = default;

    /**
     * @brief Wirtualna metoda do rysowania obiektu.
     * @param renderer Wskaźnik na obiekt PrimitiveRenderer do rysowania.
     */
    virtual void draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) = 0;
};

#endif
