#include "PrimitiveRenderer.h"



void PrimitiveRenderer::drawPoints(float* vertices, float* colors, int count) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    glDrawArrays(GL_POINTS, 0, count);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void PrimitiveRenderer::drawLines(float* vertices, float* colors, int count) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    glDrawArrays(GL_LINES, 0, count);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void PrimitiveRenderer::drawTriangles(float* vertices, float* colors, int count) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    glDrawArrays(GL_TRIANGLES, 0, count);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void PrimitiveRenderer::drawIndexedTriangles(const float* vertices, const float* colors, const unsigned int* indices, int indexCount) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void PrimitiveRenderer::drawCube(float size, float x, float y, float z, float* color) {
    float vertices[] = {
        x - size, y - size, z - size,
        x + size, y - size, z - size,
        x + size, y + size, z - size,
        x - size, y + size, z - size,
        x - size, y - size, z + size,
        x + size, y - size, z + size,
        x + size, y + size, z + size,
        x - size, y + size, z + size
    };

    float colors[] = {
        color[0], color[1], color[2],
        color[0], color[1], color[2],
        color[0], color[1], color[2],
        color[0], color[1], color[2],
        color[0], color[1], color[2],
        color[0], color[1], color[2],
        color[0], color[1], color[2],
        color[0], color[1], color[2]
    };

    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0, // przod
        4, 5, 6,  6, 7, 4, // tyl
        0, 1, 5,  5, 4, 0, // dol
        2, 3, 7,  7, 6, 2, // gora
        0, 3, 7,  7, 4, 0, // lwy
        1, 2, 6,  6, 5, 1  // prawy
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void PrimitiveRenderer::configureMaterial(const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular, float shininess) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, glm::value_ptr(ambient));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glm::value_ptr(diffuse));
    glMaterialfv(GL_FRONT, GL_SPECULAR, glm::value_ptr(specular));
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glColorMaterial(GL_FRONT, GL_AMBIENT);

}