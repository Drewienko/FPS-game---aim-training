#ifndef TARGET_OBJECT_H
#define TARGET_OBJECT_H

#include "ModelObject.h"
#include <glm/glm.hpp>

class TargetObject : public ModelObject {
public:
    TargetObject(const std::string& path);

    bool isHitByRay(const glm::vec3& rayOrigin, const glm::vec3& rayDir) const;
    void onHit();


    glm::vec3 bboxMin;
    glm::vec3 bboxMax;
private:
    void calculateBoundingBox();
};

#endif // TARGET_OBJECT_H
