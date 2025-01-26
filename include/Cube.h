#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <array>
#include "DrawableObject.h"
#include "TransformableObject.h"
#include "PrimitiveRenderer.h"

class Cube : public DrawableObject, public TransformableObject {
public:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    Cube(float size, float x, float y, float z, const float* color);
    void setupBuffers();
    void draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) override;
    void translate(const glm::vec3& direction) override;
    void rotate(float angle, const glm::vec3& axis) override;
    void rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) override;
    void scale(float sx, float sy) override;
    void setTextureForSide(int side, GLuint textureID);

private:
    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    std::vector<float> colors;
    std::array<GLuint, 6> textures = { 0, 0, 0, 0, 0, 0 };
};

#endif