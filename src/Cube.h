#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <array>
#include "DrawableObject.h"
#include <GL/gl.h>

class Cube : public DrawableObject {
public:
    Cube(float size, float x, float y, float z, const float* color);
    void draw() override;
    void setTextureForSide(int side, GLuint textureID);

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> colors;
    std::array<GLuint, 6> textures = { 0, 0, 0, 0, 0, 0 };
};

#endif