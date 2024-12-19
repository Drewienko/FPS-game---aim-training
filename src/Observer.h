#ifndef OBSERVER_H
#define OBSERVER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Observer {
public:
    Observer(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);

    glm::mat4 getViewMatrix() const;
    void move(const glm::vec3& direction);
    void rotate(float angle, const glm::vec3& axis);
    void setPosition(const glm::vec3& newPosition);
    void setTarget(const glm::vec3& newTarget);

    const glm::vec3& getPosition() const;
    const glm::vec3& getTarget() const;

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
};

#endif