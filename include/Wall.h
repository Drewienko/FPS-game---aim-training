#ifndef WALL_H
#define WALL_H

#include <vector>
#include <array>
#include "DrawableObject.h"
#include "TransformableObject.h"
#include "PrimitiveRenderer.h"

class Wall : public DrawableObject, public TransformableObject {
public:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    Wall(float width, float height,float x, float y, float z, GLuint texture);
    void setupBuffers();
    void draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) override;
    void translate(const glm::vec3& direction) override;
    void rotate(float angle, const glm::vec3& axis) override;
    void rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) override;
    void scale(float sx, float sy) override;
    void rotateAround(float angle, const glm::vec3& axis);
private:
    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    GLuint textureID;
    glm::vec3 position;
    glm::vec2 size;
};

#endif // WALL_H