#include "PrimitiveRenderer.h"
#include <GL/freeglut.h>

void PrimitiveRenderer::drawSphere(float radius, int slices, int stacks) {
    glPushMatrix();
    glutSolidSphere(radius, slices, stacks);
    glPopMatrix();
}

void PrimitiveRenderer::drawCone(float base, float height, int slices, int stacks) {
    glPushMatrix();
    glutSolidCone(base, height, slices, stacks);
    glPopMatrix();
}

void PrimitiveRenderer::drawTorus(float innerRadius, float outerRadius, int sides, int rings) {
    glPushMatrix();
    glutSolidTorus(innerRadius, outerRadius, sides, rings);
    glPopMatrix();
}

void PrimitiveRenderer::drawCube(float size) {
    glPushMatrix();
    glutSolidCube(size);
    glPopMatrix();
}

void PrimitiveRenderer::drawTeapot(float size) {
    glPushMatrix();
    glutSolidTeapot(size);
    glPopMatrix();
}

void PrimitiveRenderer::setColor(float r, float g, float b) {
    glColor3f(r, g, b);
}