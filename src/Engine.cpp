#include "Engine.h"
#include "PrimitiveRenderer.h"
#include <GL/freeglut.h>
#include <iostream>

// Definicje statycznych zmiennych
bool Engine::isPerspective = true;
int Engine::windowWidth = 800;
int Engine::windowHeight = 600;

Engine::Engine(int argc, char** argv, int width, int height, const char* title) {
    // Inicjalizacja FreeGLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);

    initSettings();

    glutDisplayFunc(displayCallback);
    glutKeyboardFunc(keyboardCallback);
    glutReshapeFunc(reshapeCallback);
    glutTimerFunc(1000 / 60, timerCallback, 0); // 60 FPS
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

    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    PrimitiveRenderer::setColor(1.0f, 0.0f, 0.0f); 
    PrimitiveRenderer::drawSphere(1.0f, 20, 20);

    PrimitiveRenderer::setColor(0.0f, 1.0f, 0.0f);
    glTranslatef(3.0f, 0.0f, 0.0f);
    PrimitiveRenderer::drawCube(1.5f);

    PrimitiveRenderer::setColor(0.0f, 0.0f, 1.0f);
    glTranslatef(-6.0f, 0.0f, 0.0f);
    PrimitiveRenderer::drawTeapot(1.0f);

    glutSwapBuffers();
}

void Engine::keyboardCallback(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    } else if (key == 'p') {
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
    } else {
        glOrtho(-2.0, 2.0, -2.0, 2.0, 1.0, 100.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}