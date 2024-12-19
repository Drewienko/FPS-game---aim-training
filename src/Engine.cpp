#include "Engine.h"
#include "PrimitiveRenderer.h"
#include "Observer.h"
#include "Cube.h"
#include <GL/freeglut.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Definicje statycznych zmiennych
bool Engine::isPerspective = true;
int Engine::windowWidth = 800;
int Engine::windowHeight = 600;
Observer* Engine::observer = nullptr;
static bool isMousePressed = false;
static int lastMouseX = -1;
static int lastMouseY = -1;

Engine::Engine(int argc, char** argv, int width, int height, const char* title) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);

    initSettings();

    observer = new Observer(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glutDisplayFunc(displayCallback);
    glutKeyboardFunc(keyboardCallback);
    glutReshapeFunc(reshapeCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotionCallback);
    glutTimerFunc(1000 / 60, timerCallback, 0);
}

Engine::~Engine() {
    delete observer;
}

void Engine::mouseCallback(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isMousePressed = true;
            lastMouseX = x;
            lastMouseY = y;
        }
        else if (state == GLUT_UP) {
            isMousePressed = false;
            lastMouseX = -1;
            lastMouseY = -1;
        }
    }
}

void Engine::mouseMotionCallback(int x, int y) {
    if (!isMousePressed || lastMouseX == -1 || lastMouseY == -1) {
        return;
    }

    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;

    observer->rotate(deltaX * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
    observer->rotate(deltaY * 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}


void Engine::keyboardCallback(unsigned char key, int x, int y) {
    float speed = 0.5f;
    if (key == 'w') {
        observer->translate(glm::vec3(0.0f, 0.0f, -speed));
    }
    else if (key == 's') {
        observer->translate(glm::vec3(0.0f, 0.0f, speed));
    }
    else if (key == 'a') {
        observer->translate(glm::vec3(-speed, 0.0f, 0.0f));
    }
    else if (key == 'd') {
        observer->translate(glm::vec3(speed, 0.0f, 0.0f));
    }
    else if (key == 'q') {
        observer->translate(glm::vec3(0.0f, speed, 0.0f));
    }
    else if (key == 'e') {
        observer->translate(glm::vec3(0.0f, -speed, 0.0f));
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
    glLoadMatrixf(glm::value_ptr(observer->getViewMatrix()));

    // punkty
    float pointVertices[] = {
        -3.0f, 3.0f, 0.0f,
        -2.0f, 3.0f, 0.0f
    };
    float pointColors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    PrimitiveRenderer::drawPoints(pointVertices, pointColors, 2);

    // linie
    float lineVertices[] = {
        -3.0f, 2.0f, 0.0f,
        -1.0f, 2.0f, 0.0f
    };
    float lineColors[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };
    PrimitiveRenderer::drawLines(lineVertices, lineColors, 2);

    // trojkaty
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

    // wachlarze
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

    // czworokaty
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

    float cubeColors[] = { 0.0f, 1.0f, 0.0f };
    Cube *cube = new Cube(1.0f, 0.0f, 3.0f, 0.0f, cubeColors);
    cube->draw();


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