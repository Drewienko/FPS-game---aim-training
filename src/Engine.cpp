#include "Engine.h"
#include "PrimitiveRenderer.h"
#include <GL/freeglut.h>
#include <iostream>

// Definicje statycznych zmiennych
bool Engine::isPerspective = true;
int Engine::windowWidth = 800;
int Engine::windowHeight = 600;

float Engine::cameraX = 0.0f;
float Engine::cameraY = 0.0f;
float Engine::cameraZ = 10.0f;
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
    glutMotionFunc(mouseMotionCallback); // uruchamia sie tylko jesli jeden z przyciskow jest wcisniety
    glutTimerFunc(1000 / 60, timerCallback, 0); // 60 FPS
}

void Engine::mouseMotionCallback(int x, int y) {
    if (lastMouseX >= 0 && lastMouseY >= 0) {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        cameraAngleX += deltaX * 0.1f;
        cameraAngleY += deltaY * 0.1f;
        if (cameraAngleY > 89.0f) cameraAngleY = 89.0f; // blokada obrotu
        if (cameraAngleY < -89.0f) cameraAngleY = -89.0f;
    }

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}

void Engine::keyboardCallback(unsigned char key, int x, int y) {
    float speed = 0.5f;
    if (key == 'w') {
        cameraZ -= speed;
    }
    else if (key == 's') {
        cameraZ += speed;
    }
    else if (key == 'a') {
        cameraX -= speed;
    }
    else if (key == 'd') {
        cameraX += speed;
    }
    else if (key == 'q') {
        cameraY += speed;
    }
    else if (key == 'e') {
        cameraY -= speed;
    }
    else if (key == 27) { // ESC
        exit(0);
    }

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

    glTranslatef(-cameraX, -cameraY, -cameraZ);
    glRotatef(cameraAngleY, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleX, 0.0f, 1.0f, 0.0f);

    // Example 1: Drawing points
    float pointVertices[] = {
        -3.0f, 3.0f, 0.0f,
        -2.0f, 3.0f, 0.0f
    };
    float pointColors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    PrimitiveRenderer::drawPoints(pointVertices, pointColors, 2);

    // Example 2: Drawing lines
    float lineVertices[] = {
        -3.0f, 2.0f, 0.0f,
        -1.0f, 2.0f, 0.0f
    };
    float lineColors[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };
    PrimitiveRenderer::drawLines(lineVertices, lineColors, 2);

    // Example 3: Drawing triangles
    float triangleVertices[] = {
        -3.0f, 1.0f, 0.0f,
        -2.0f, 1.5f, 0.0f,
        -1.0f, 1.0f, 0.0f
    };
    float triangleColors[] = {
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f
    };
    PrimitiveRenderer::drawTriangles(triangleVertices, triangleColors, 3);

    // Example 4: Drawing triangle fan
    float fanVertices[] = {
        0.0f, 0.0f, 0.0f,
        0.5f, 1.0f, 0.0f,
        1.0f, 0.5f, 0.0f,
        1.0f, -0.5f, 0.0f,
        0.5f, -1.0f, 0.0f
    };
    float fanColors[] = {
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f
    };
    PrimitiveRenderer::drawTriangles(fanVertices, fanColors, 5);

    // Example 5: Drawing quads
    float quadVertices[] = {
        2.0f, 1.0f, 0.0f,
        3.0f, 1.0f, 0.0f,
        3.0f, 0.0f, 0.0f,
        2.0f, 0.0f, 0.0f
    };
    float quadColors[] = {
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f
    };
    PrimitiveRenderer::drawTriangles(quadVertices, quadColors, 4);

    glutSwapBuffers();
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
        glOrtho(-10.0, 10.0, -10.0, 10.0, 1.0, 100.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}