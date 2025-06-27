// TargetObject.cpp
#include "TargetObject.h"
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

TargetObject::TargetObject(const std::string& path)
    : ModelObject(path) {
    calculateBoundingBox();
}

void TargetObject::calculateBoundingBox() {
    bool first = true;
    for (const Mesh& mesh : meshes) {
        for (const Vertex& v : mesh.vertices) {
            glm::vec3 p = v.position;
            if (first) {
                bboxMin = bboxMax = p;
                first = false;
            }
            else {
                bboxMin = glm::min(bboxMin, p);
                bboxMax = glm::max(bboxMax, p);
            }
        }
    }
}

bool TargetObject::isHitByRay(const glm::vec3& rayOrigin, const glm::vec3& rayDir) const {
    glm::vec3 a = glm::vec3(getModelMatrix() * glm::vec4(bboxMin, 1.0f));
    glm::vec3 b = glm::vec3(getModelMatrix() * glm::vec4(bboxMax, 1.0f));
    glm::vec3 min = glm::min(a, b);
    glm::vec3 max = glm::max(a, b);

    float tmin = 0.0f, tmax = 1000.0f;

    for (int i = 0; i < 3; ++i) {
        float invD = 1.0f / rayDir[i];
        float t0 = (min[i] - rayOrigin[i]) * invD;
        float t1 = (max[i] - rayOrigin[i]) * invD;
        if (invD < 0.0f) std::swap(t0, t1);
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
        if (tmax <= tmin) return false;
    }

    return true;
}


void TargetObject::onHit() {
    std::cout << "Target hit!" << std::endl;
    
}