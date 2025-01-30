#ifndef DRAWABLEOBJECT_H
#define DRAWABLEOBJECT_H

#include "GameObject.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

/**
 * @class DrawableObject
 * @brief Klasa bazowa dla obiektów, które mogą być rysowane.
 *
 * Klasa ta definiuje interfejs dla wszystkich obiektów, które mogą być
 * renderowane w scenie 3D. Umożliwia implementację własnych metod rysowania
 * w klasach pochodnych.
 */
class DrawableObject : virtual public GameObject
{
public:
    /**
     * @brief Wirtualny destruktor klasy DrawableObject.
     *
     * Zapewnia poprawne zarządzanie pamięcią w klasach pochodnych.
     */
    virtual ~DrawableObject() = default;

    /**
     * @brief Rysuje obiekt przy użyciu podanego programu cieniującego i macierzy transformacji.
     *
     * Ta metoda powinna być nadpisana w klasach pochodnych, aby umożliwić renderowanie
     * specyficznych obiektów w scenie. Wykorzystuje program cieniujący oraz przekazane
     * macierze modelu, widoku i projekcji do prawidłowego rysowania obiektu w przestrzeni 3D.
     *
     * @param shaderProgram Identyfikator programu cieniującego OpenGL.
     * @param model Macierz modelu, określająca transformację obiektu w przestrzeni świata.
     * @param view Macierz widoku, określająca pozycję kamery i jej orientację.
     * @param projection Macierz projekcji, definiująca sposób odwzorowania 3D na 2D.
     */
    virtual void draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) = 0;
};

#endif // DRAWABLEOBJECT_H
