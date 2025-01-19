#include "Wall.h"
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "DrawableObject.h"
#include "TransformableObject.h"

Wall::Wall(float x, float y, float z, float width, float height, GLuint texture)
    : position(x, y, z), size(width, height), textureID(texture) {
    vertices[0] = x;         vertices[1] = y;         vertices[2] = z;
    vertices[3] = x + width; vertices[4] = y;         vertices[5] = z;
    vertices[6] = x + width; vertices[7] = y + height; vertices[8] = z;
    vertices[9] = x;         vertices[10] = y + height; vertices[11] = z;
}

void Wall::draw() {
    glPushMatrix();

    glDisable(GL_COLOR_MATERIAL);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

 
    glNormal3f(0.0f, 0.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[0], vertices[1], vertices[2]);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[3], vertices[4], vertices[5]);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[6], vertices[7], vertices[8]);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[9], vertices[10], vertices[11]);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Wall::translate(const glm::vec3& direction) {
    position += direction;
    for (int i = 0; i < 4; ++i) {
        vertices[i * 3] += direction.x;
        vertices[i * 3 + 1] += direction.y;
        vertices[i * 3 + 2] += direction.z;
    }
}

void Wall::rotate(float angle, const glm::vec3& axis) {
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    for (int i = 0; i < 4; ++i) {
        glm::vec4 vertex(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2], 1.0f);
        vertex = rotationMatrix * vertex;
        vertices[i * 3] = vertex.x;
        vertices[i * 3 + 1] = vertex.y;
        vertices[i * 3 + 2] = vertex.z;
    }
}

void Wall::rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) {
    this->translate(-point);

    this->rotate(angle, axis);

    this->translate(point);
}

void Wall::rotateAround(float angle, const glm::vec3& axis) {
    float x=0.0, y = 0.0, z = 0.0;
    for (int i = 0; i < 4; ++i) {
        x += vertices[i * 3];
        y += vertices[i * 3 + 1];
        z += vertices[i * 3 + 2];
    }
    glm::vec3 point(x/4.0f, y / 4.0f, z / 4.0f);

    this->translate(-point);

    this->rotate(angle, axis);

    this->translate(point);
}

void Wall::scale(float sx, float sy) {
    size.x *= sx;
    size.y *= sy;
    vertices[3] = vertices[0] + size.x;
    vertices[6] = vertices[0] + size.x;
    vertices[7] = vertices[1] + size.y;
    vertices[10] = vertices[1] + size.y;
}
