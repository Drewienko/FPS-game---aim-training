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
        textures[i] = 0; 
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

    glEnable(GL_TEXTURE_2D); 
}


void Cube::setTextureForSide(int side, GLuint textureID) {
    if (side >= 0 && side < 6) {
        textures[side] = textureID;
    }
}

void Cube::translate(const glm::vec3& direction) {
    for (size_t i = 0; i < vertices.size(); i += 3) {
        vertices[i] += direction.x;
        vertices[i + 1] += direction.y;
        vertices[i + 2] += direction.z;
    }
}

void Cube::rotate(float angle, const glm::vec3& axis) {
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    for (size_t i = 0; i < vertices.size(); i += 3) {
        glm::vec4 vertex(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);
        vertex = rotationMatrix * vertex;
        vertices[i] = vertex.x;
        vertices[i + 1] = vertex.y;
        vertices[i + 2] = vertex.z;
    }
}

void Cube::rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) {
    this->translate(-point);

    this->rotate(angle, axis);

    this->translate(point);
}

void Cube::scale(float sx, float sy) {
    glm::vec3 center(0.0f);
    for (size_t i = 0; i < vertices.size(); i += 3) {
        center.x += vertices[i];
        center.y += vertices[i + 1];
        center.z += vertices[i + 2];
    }
    center /= static_cast<float>(vertices.size() / 3);

    for (size_t i = 0; i < vertices.size(); i += 3) {
        vertices[i] = center.x + (vertices[i] - center.x) * sx;
        vertices[i + 1] = center.y + (vertices[i + 1] - center.y) * sy;
    }
}

