#ifndef OBSERVER_H
#define OBSERVER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "TransformableObject.h"

class Observer : public TransformableObject {
public:
    Observer(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);

    glm::mat4 getViewMatrix() const;
    void translate(const glm::vec3& direction);
    void rotate(float angle, const glm::vec3& axis);
    void rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) override;
    void scale(float sx, float sy) {};
    void setPosition(const glm::vec3& newPosition);
    void setTarget(const glm::vec3& newTarget);

    const glm::vec3& getPosition() const;
    const glm::vec3& getTarget() const;

    void moveForward(float distance);

    void moveRight(float distance);

    float getPitch() const;
    float getYaw() const;
    void setPitch(float newPitch);
    void setYaw(float newYaw);

    void updateTarget(); // Recalculate target based on pitch and yaw

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    float pitch; // Vertical rotation in degrees
    float yaw;   // Horizontal rotation in degrees
};

#endif