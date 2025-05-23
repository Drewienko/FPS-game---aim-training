#include "Observer.h"

Observer::Observer(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    : position(position), target(target), up(up) {
    glm::vec3 direction = glm::normalize(target - position);
    pitch = glm::degrees(asin(direction.y));
    yaw = glm::degrees(atan2(direction.z, direction.x));
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

float Observer::getPitch() const {
    return pitch;
}

float Observer::getYaw() const {
    return yaw;
}

void Observer::setPitch(float newPitch) {
    pitch = glm::clamp(newPitch, -89.0f, 89.0f);
    updateTarget();
}

void Observer::setYaw(float newYaw) {
    yaw = glm::mod(newYaw, 360.0f);
    updateTarget();
}

void Observer::updateTarget() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    direction.y = sin(glm::radians(pitch));
    direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

    target = position + glm::normalize(direction);
}

void Observer::applyMovementInput(const std::set<char>& keys, float deltaTime, const std::vector<Wall*>& world) {
    glm::vec3 forward = glm::normalize(target - position);
    forward.y = 0.0f;
    glm::vec3 right = glm::normalize(glm::cross(forward, up));

    glm::vec3 moveDir(0.0f);
    if (keys.count('w')) moveDir += forward;
    if (keys.count('s')) moveDir -= forward;
    if (keys.count('d')) moveDir += right;
    if (keys.count('a')) moveDir -= right;

    if (glm::length(moveDir) > 0.0f)
        moveDir = glm::normalize(moveDir);

    glm::vec3 proposedMove = moveDir * speed * deltaTime;
    glm::vec3 proposedPos = position + glm::vec3(proposedMove.x, proposedMove.y, proposedMove.z);

    glm::vec3 dir = proposedMove;
    if (glm::length(dir) > 0.0f)
        dir = glm::normalize(dir);
    else
        dir = glm::vec3(0.0f);

    bool collision = false;
    for (const auto& wall : world) {
        float dist;
        glm::vec3 min = wall->getMinBounds();
        glm::vec3 max = wall->getMaxBounds();

        if (rayIntersectsAABB(position, dir, min, max, dist)) {
            if (dist > 0.01f && dist < glm::length(proposedMove) + 0.05f) {
                collision = true;
                break;
            }
        }
    }

    if (!collision) {
        position += glm::vec3(proposedMove.x, 0.0f, proposedMove.z);
        target += glm::vec3(proposedMove.x, 0.0f, proposedMove.z);
    }

    if (keys.count(' ') && onGround) {
        velocity.y = 5.5f;
        onGround = false;
    }
}


void Observer::updatePhysics(float deltaTime, const std::vector<Wall*>& world) {
    float heightOffset = 1.8f; 
    velocity.y -= 9.81f * deltaTime;
    glm::vec3 nextPos = position + velocity * deltaTime;

    onGround = false;

    glm::vec3 rayOrigin = position - glm::vec3(0.0f, heightOffset, 0.0f);
    glm::vec3 rayDir = glm::vec3(0.0f, -1.0f, 0.0f);
    float maxDist = 0.25f;

    for (const auto& wall : world) {
        float hitDist;
        glm::vec3 min = wall->getMinBounds();
        glm::vec3 max = wall->getMaxBounds();

        bool hit = rayIntersectsAABB(rayOrigin, rayDir, min, max, hitDist);
        if (hit && hitDist <= maxDist && hitDist >= -0.01f) {
            onGround = true;
            velocity.y = 0.0f;

            float feetY = max.y + 0.01f;
            position.y = feetY + heightOffset;

            break;
        }
    }

    if (!onGround) {
        position.y = nextPos.y;
    }

    target.y = position.y + sin(glm::radians(pitch));
}






bool Observer::rayIntersectsAABB(const glm::vec3& origin,
    const glm::vec3& dir,
    const glm::vec3& aabbMin,
    const glm::vec3& aabbMax,
    float& t) {
    float tmin = -FLT_MAX;
    float tmax = FLT_MAX;

    for (int i = 0; i < 3; ++i) {
        if (std::abs(dir[i]) < 1e-6f) {
            // Ray is parallel to slab. No hit if origin not within the slab
            if (origin[i] < aabbMin[i] || origin[i] > aabbMax[i])
                return false;
        }
        else {
            float ood = 1.0f / dir[i];
            float t1 = (aabbMin[i] - origin[i]) * ood;
            float t2 = (aabbMax[i] - origin[i]) * ood;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }

    t = tmin;
    return true;
}


