#ifndef OBSERVER_H
#define OBSERVER_H

#include "TransformableObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "TargetObject.h"
#include <vector>
#include <set>
#include "Wall.h"
/**
 * @class Observer
 * @brief Klasa reprezentująca obserwatora (kamerę) w przestrzeni 3D.
 *
 * Klasa ta dziedziczy po TransformableObject, co umożliwia jej translację i rotację w przestrzeni 3D.
 * Implementuje mechanizmy pozwalające na ruch kamery oraz zmianę kierunku patrzenia.
 */
class Observer : public TransformableObject {
public:
    /**
     * @brief Konstruktor tworzący obserwatora w określonej pozycji i z określonym kierunkiem patrzenia.
     *
     * @param position Początkowa pozycja obserwatora.
     * @param target Punkt, na który patrzy obserwator.
     * @param up Wektor określający górę (zwykle {0, 1, 0}).
     */
    Observer(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);

    /**
     * @brief Zwraca macierz widoku obliczoną na podstawie pozycji i kierunku patrzenia.
     *
     * @return Macierz widoku używana do ustawienia kamery w OpenGL.
     */
    glm::mat4 getViewMatrix() const;

    /**
     * @brief Przesuwa obserwatora wzdłuż podanego wektora kierunku.
     *
     * @param direction Wektor przesunięcia (x, y, z).
     */
    void translate(const glm::vec3& direction);

    /**
     * @brief Obraca obserwatora o zadany kąt wokół podanej osi.
     *
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     */
    void rotate(float angle, const glm::vec3& axis);

    /**
     * @brief Obraca obserwatora wokół określonego punktu.
     *
     * @param angle Kąt obrotu w stopniach.
     * @param axis Wektor osi obrotu.
     * @param point Punkt, wokół którego wykonywany jest obrót.
     */
    void rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) override;

    /**
     * @brief Metoda pusta, ponieważ obserwator nie obsługuje skalowania.
     */
    void scale(float sx, float sy) {};

    /**
     * @brief Ustawia nową pozycję obserwatora.
     *
     * @param newPosition Nowa pozycja w przestrzeni 3D.
     */
    void setPosition(const glm::vec3& newPosition);

    /**
     * @brief Ustawia nowy punkt, na który patrzy obserwator.
     *
     * @param newTarget Nowa pozycja celu.
     */
    void setTarget(const glm::vec3& newTarget);

    /**
     * @brief Pobiera aktualną pozycję obserwatora.
     *
     * @return Referencja do wektora pozycji.
     */
    const glm::vec3& getPosition() const;

    /**
     * @brief Pobiera aktualny punkt, na który patrzy obserwator.
     *
     * @return Referencja do wektora celu.
     */
    const glm::vec3& getTarget() const;

    /**
     * @brief Przesuwa obserwatora do przodu o określoną odległość.
     *
     * @param distance Odległość przesunięcia.
     */
    void moveForward(float distance);

    /**
     * @brief Przesuwa obserwatora w prawo o określoną odległość.
     *
     * @param distance Odległość przesunięcia.
     */
    void moveRight(float distance);

    /**
     * @brief Pobiera aktualny kąt nachylenia (pitch) obserwatora.
     *
     * @return Wartość kąta pitch w stopniach.
     */
    float getPitch() const;

    /**
     * @brief Pobiera aktualny kąt obrotu (yaw) obserwatora.
     *
     * @return Wartość kąta yaw w stopniach.
     */
    float getYaw() const;

    /**
     * @brief Ustawia nowy kąt nachylenia (pitch).
     *
     * @param newPitch Nowa wartość kąta pitch w stopniach.
     */
    void setPitch(float newPitch);

    /**
     * @brief Ustawia nowy kąt obrotu (yaw).
     *
     * @param newYaw Nowa wartość kąta yaw w stopniach.
     */
    void setYaw(float newYaw);

    /**
     * @brief Aktualizuje wektor celu na podstawie wartości pitch i yaw.
     *
     * Metoda ta przelicza nowy wektor kierunku patrzenia na podstawie aktualnych wartości
     * kątów pitch i yaw, umożliwiając płynne obracanie kamery.
     */
    void updateTarget();

    glm::vec3 velocity = glm::vec3(0.0f);
    float speed = 5.0f;
    bool onGround = false;

    void applyMovementInput(const std::set<char>& keys, float deltaTime, const std::vector<Wall*>& world);
    void updatePhysics(float deltaTime, const std::vector<Wall*>& world);
    bool rayIntersectsAABB(const glm::vec3& origin,
        const glm::vec3& dir,
        const glm::vec3& aabbMin,
        const glm::vec3& aabbMax,
        float& t);

private:
    /**
     * @brief Aktualna pozycja obserwatora w przestrzeni 3D.
     */
    glm::vec3 position;

    /**
     * @brief Aktualny punkt, na który patrzy obserwator.
     */
    glm::vec3 target;

    /**
     * @brief Wektor określający górę (up direction).
     */
    glm::vec3 up;

    /**
     * @brief Kąt nachylenia (pitch) obserwatora w stopniach.
     */
    float pitch;

    /**
     * @brief Kąt obrotu (yaw) obserwatora w stopniach.
     */
    float yaw;
};

#endif // OBSERVER_H
