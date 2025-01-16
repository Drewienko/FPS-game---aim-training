#include "Observer.h"

Observer::Observer(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    : position(position), target(target), up(up) {
}

glm::mat4 Observer::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

void Observer::translate(const glm::vec3& direction) {
    position += direction;
    target += direction;
}

void Observer::rotate(float angle, const glm::vec3& axis) {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    glm::vec4 newTarget = rotation * glm::vec4(target - position, 1.0f);
    target = position + glm::vec3(newTarget);
}

void Observer::rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) {
    this->translate(-point);

    this->rotate(angle, axis);

    this->translate(point);
}

void Observer::setPosition(const glm::vec3& newPosition) {
    target += (newPosition - position);
    position = newPosition;
}

void Observer::setTarget(const glm::vec3& newTarget) {
    target = newTarget;
}

const glm::vec3& Observer::getPosition() const {
    return position;
}

const glm::vec3& Observer::getTarget() const {
    return target;
}
