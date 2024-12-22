#include "Engine.h"


// Definicje statycznych zmiennych
bool Engine::isPerspective = true;
int Engine::windowWidth = 800;
int Engine::windowHeight = 600;
Observer* Engine::observer = nullptr;
static bool isMousePressed = false;
static int lastMouseX = -1;
static int lastMouseY = -1;
static GLenum shadingMode = GL_SMOOTH;


Observer* observer = nullptr;

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

void Engine::initSettings() {
    glEnable(GL_DEPTH_TEST);
    setClearColor(0.0f, 0.0f, 0.0f);

    initLighting();
    glShadeModel(shadingMode);
}

void Engine::initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPosition[] = { 10.0f, 10.0f, 10.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_COLOR_MATERIAL);

}

void Engine::displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadMatrixf(glm::value_ptr(observer->getViewMatrix()));


    float pointVertices[] = {
        -3.0f, 3.0f, 0.0f,
        -2.0f, 3.0f, 0.0f
    };
    float pointColors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    PrimitiveRenderer::drawPoints(pointVertices, pointColors, 2);

    float lineVertices[] = {
        -3.0f, 2.0f, 0.0f,
        -1.0f, 2.0f, 0.0f
    };
    float lineColors[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };
    PrimitiveRenderer::drawLines(lineVertices, lineColors, 2);

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


    glm::vec4 ambient(0.1f, 0.1f, 0.1f, 1.0f);
    glm::vec4 diffuse(0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec4 specular(1.0f, 1.0f, 1.0f, 1.0f);
    float shininess = 50.0f;

    PrimitiveRenderer::configureMaterial(ambient, diffuse, specular, shininess);

    float cubeSize = 1.0f;


    float cubeColor1[] = { 0.0f, 1.0f, 0.0f };
    Cube cube1(cubeSize, -2.0f, 0.0f, 0.0f, cubeColor1);
    cube1.draw();

    float cubeColor2[] = { 0.0f, 0.0f, 1.0f };
    Cube cube2(cubeSize, 2.0f, 0.0f, 0.0f, cubeColor2);
    cube2.draw();

    float cubeColor3[] = { 1.0f, 0.0f, 0.0f };
    Cube cube3(cubeSize, 0.0f, 2.0f, 0.0f, cubeColor3);
    cube3.draw();


    float cubeColor4[] = { 1.0f, 1.0f, 0.0f };
    Cube cube4(cubeSize, 0.0f, -2.0f, 0.0f, cubeColor4);
    cube4.draw();

    glutSwapBuffers();
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
    else if (key == 'f' || key == 'F') {
        shadingMode = GL_FLAT;
        glShadeModel(shadingMode);
        std::cout << "Switched to Flat Shading" << std::endl;
    }
    else if (key == 'g' || key == 'G') {
        shadingMode = GL_SMOOTH;
        glShadeModel(shadingMode);
        std::cout << "Switched to Smooth Shading (Gouraud)" << std::endl;
    }

    glutPostRedisplay();
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

void Engine::reshapeCallback(int w, int h) {
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

void Engine::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void Engine::start() {
    glutMainLoop();
}