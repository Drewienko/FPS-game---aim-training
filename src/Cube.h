#ifndef CUBE_H
#define CUBE_H

#include <vector>

class Cube {
public:
    Cube(float size, float x, float y, float z, const float* color);
    void draw();

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> colors;
};

#endif
