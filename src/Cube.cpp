#include "Cube.h"
#include "PrimitiveRenderer.h"

Cube::Cube(float size, float x, float y, float z, const float* color) 
{
    vertices = {
        x - size, y - size, z - size,
        x + size, y - size, z - size,
        x + size, y + size, z - size,
        x - size, y + size, z - size,
        x - size, y - size, z + size,
        x + size, y - size, z + size,
        x + size, y + size, z + size,
        x - size, y + size, z + size
    };

    indices = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        0, 1, 5,  5, 4, 0,
        2, 3, 7,  7, 6, 2,
        0, 3, 7,  7, 4, 0,
        1, 2, 6,  6, 5, 1
    };

    for (int i = 0; i < 8; ++i) {
        colors.push_back(color[0]);
        colors.push_back(color[1]);
        colors.push_back(color[2]);
    }
}

void Cube::draw(){
    PrimitiveRenderer::drawIndexedTriangles(vertices.data(), colors.data(), indices.data(), static_cast<int>(indices.size()));

}