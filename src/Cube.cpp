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

    for (int i = 0; i < 6; ++i) {
        textures[i] = 0; // Initialize textures with default value
    }
}

void Cube::draw() {
    for (int i = 0; i < 6; ++i) {
        if (textures[i] != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
        }
        else {
            glDisable(GL_TEXTURE_2D);
            glColor3f(colors[0], colors[1], colors[2]);
        }

        glBegin(GL_TRIANGLES);
        for (int j = 0; j < 6; ++j) {
            int vertexIndex = indices[i * 6 + j] * 3;
            float texCoordX = (j == 1 || j == 2 || j == 4) ? 1.0f : 0.0f;
            float texCoordY = (j >= 2 && j <= 4) ? 1.0f : 0.0f;
            glTexCoord2f(texCoordX, texCoordY);
            glVertex3f(vertices[vertexIndex], vertices[vertexIndex + 1], vertices[vertexIndex + 2]);
        }
        glEnd();
    }

    glEnable(GL_TEXTURE_2D); // Ensure texture is re-enabled for subsequent rendering
}

void Cube::setTextureForSide(int side, GLuint textureID) {
    if (side >= 0 && side < 6) {
        textures[side] = textureID;
    }
}