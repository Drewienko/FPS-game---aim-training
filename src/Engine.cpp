#include "Engine.h"
#include "PrimitiveRenderer.h"
#include <GL/freeglut.h>
#include <iostream>

// Definicje statycznych zmiennych
bool Engine::isPerspective = true;
int Engine::windowWidth = 800;
int Engine::windowHeight = 600;

float Engine::cameraAngleX = 0.0f;
float Engine::cameraAngleY = 0.0f;
int Engine::lastMouseX = -1;
int Engine::lastMouseY = -1;

Engine::Engine(int argc, char** argv, int width, int height, const char* title) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);

    initSettings();

    glutDisplayFunc(displayCallback);
    glutKeyboardFunc(keyboardCallback);
    glutReshapeFunc(reshapeCallback);
    glutMotionFunc(mouseMotionCallback); //uruchamia sie tylko jesli jeden z przyciskow jest wcisniety
    glutTimerFunc(1000 / 60, timerCallback, 0); // 60 FPS
}

void Engine::mouseMotionCallback(int x, int y) {
    if (lastMouseX >= 0 && lastMouseY >= 0) {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        cameraAngleX += deltaX * 0.1f;
        cameraAngleY += deltaY * 0.1f;
        if (cameraAngleY > 89.0f) cameraAngleY = 89.0f; //blokada obrotu
        if (cameraAngleY < -89.0f) cameraAngleY = -89.0f;
    }

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay(); 
}

void Engine::start() {
    glutMainLoop();
}

void Engine::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void Engine::changeProjection(bool perspective) {
    isPerspective = perspective;
    updateProjectionMatrix();
}

void Engine::initSettings() {
    glEnable(GL_DEPTH_TEST);
    setClearColor(0.0f, 0.0f, 0.0f);
    updateProjectionMatrix();
}

void Engine::displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glRotatef(cameraAngleY, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleX, 0.0f, 1.0f, 0.0f);
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    PrimitiveRenderer::setColor(1.0f, 0.0f, 0.0f);
    PrimitiveRenderer::drawSphere(1.0f, 20, 20);

    PrimitiveRenderer::setColor(0.0f, 1.0f, 0.0f);
    glTranslatef(3.0f, 0.0f, 0.0f);
    PrimitiveRenderer::drawCube(1.5f);

    PrimitiveRenderer::setColor(0.0f, 1.0f, 0.0f);
    glTranslatef(3.0f, 0.0f, 0.0f);
    PrimitiveRenderer::drawTorus(1.5f,1.0f,3,3);

    PrimitiveRenderer::setColor(0.0f, 1.0f, 1.0f);
    glTranslatef(-9.0f, 0.0f, 0.0f);
    PrimitiveRenderer::drawCone(2.0f,1.0f,3,5);

    PrimitiveRenderer::setColor(0.0f, 0.0f, 1.0f);
    glTranslatef(-3.0f, 0.0f, 0.0f);
    PrimitiveRenderer::drawTeapot(1.0f);

    glutSwapBuffers();
}

void Engine::keyboardCallback(unsigned char key, int x, int y) {
    if (key == 27) { // ESC
        exit(0);
    }
    else if (key == 'p') {
        changeProjection(!isPerspective);
    }
}

void Engine::reshapeCallback(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    updateProjectionMatrix();
}

void Engine::timerCallback(int value) {
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timerCallback, value); // 60 FPS
}

void Engine::updateProjectionMatrix() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (isPerspective) {
        gluPerspective(60.0, static_cast<double>(windowWidth) / windowHeight, 1.0, 100.0);
    }
    else {
        glOrtho(-2.0, 2.0, -2.0, 2.0, 1.0, 100.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
