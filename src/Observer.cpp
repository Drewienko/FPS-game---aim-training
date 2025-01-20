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
    glm::vec3 direction = glm::normalize(target - position);
    glm::vec3 rotatedDirection = glm::vec3(rotation * glm::vec4(direction, 0.0f));
    target = position + rotatedDirection;
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

void Observer::moveForward(float distance) {
    glm::vec3 forward = glm::normalize(target - position);
    this->translate( forward * distance);
}

void Observer::moveRight(float distance) {
    glm::vec3 forward = glm::normalize(target - position);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    this->translate(right * distance);
}
