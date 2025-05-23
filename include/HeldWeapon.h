#include "ModelObject.h"

class HeldWeapon : public ModelObject {
public:
    HeldWeapon(const std::string& path);

    void translate(const glm::vec3& direction) override;
    void triggerRecoil();
    void update(float deltaTime);
    glm::vec3 defaultPosition;
private:
    float recoilTime = 0.0f;
    float maxRecoilTime = 0.15f; // czas trwania odrzutu
    float recoilOffset = 0.05f;  // odleg³oœæ cofniêcia
    bool recoiling = false;
};