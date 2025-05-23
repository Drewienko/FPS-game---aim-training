
#include "HeldWeapon.h"

HeldWeapon::HeldWeapon(const std::string& path)
    : ModelObject(path) {
    this->defaultPosition = glm::vec3(0.35f, -0.55f, -0.6f);
    setPosition(defaultPosition);
}

void HeldWeapon::triggerRecoil() {
    if (!recoiling) {
        recoiling = true;
        recoilTime = 0.0f;
    }
}

void HeldWeapon::update(float deltaTime) {
    if (!recoiling) {
        setPosition(defaultPosition);
        return;
    }

    recoilTime += deltaTime;
    float t = recoilTime / maxRecoilTime;

    if (t < 0.5f) {
        float offset = glm::mix(0.0f, -recoilOffset, t * 2.0f);
        setPosition(defaultPosition + glm::vec3(0.0f, 0.0f, offset));
    }
    else if (t < 1.0f) {
        float offset = glm::mix(-recoilOffset, 0.0f, (t - 0.5f) * 2.0f);
        setPosition(defaultPosition + glm::vec3(0.0f, 0.0f, offset));
    }
    else {
        recoiling = false;
        setPosition(defaultPosition);
    }
}

void HeldWeapon::translate(const glm::vec3& direction) {
    defaultPosition += direction;
}
