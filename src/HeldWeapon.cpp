// HeldWeapon.cpp
#include "HeldWeapon.h"

HeldWeapon::HeldWeapon(const std::string& path)
    : ModelObject(path) {
    setPosition(glm::vec3(0.35f, -0.55f, -0.6f)); // pozycja wzglêdem kamery
}

void HeldWeapon::triggerRecoil() {
    if (!recoiling) {
        recoiling = true;
        recoilTime = 0.0f;
    }
}

void HeldWeapon::update(float deltaTime) {
    if (!recoiling) return;

    recoilTime += deltaTime;
    float t = recoilTime / maxRecoilTime;

    if (t < 0.5f) {
        // cofa siê
        float offset = glm::mix(0.0f, -recoilOffset, t * 2.0f);
        setPosition(this->position + glm::vec3(0.0f, 0.0f, -offset));
    }
    else if (t < 1.0f) {
        // wraca
        float offset = glm::mix(-recoilOffset, 0.0f, (t - 0.5f) * 2.0f);
        setPosition(this->position + glm::vec3(0.0f, 0.0f, -offset));
    }
    else {
        recoiling = false;
        setPosition(glm::vec3(0.35f, -0.55f, -0.6f));
    }
}
