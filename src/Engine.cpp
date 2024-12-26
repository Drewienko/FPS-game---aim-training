#include "Engine.h"
#include "BitmapHandler.h"
#include "Cube.h"
#include "Wall.h"

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

Cube* texturedCube = nullptr;
std::vector<Cube*> cubes;
float rotationAngle = 0.0f;
Wall* testWall = nullptr;

GLfloat light1Position[] = { -10.0f, 10.0f, 10.0f, 1.0f };

Engine::Engine(int argc, char** argv, int width, int height, const char* title) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);

    initSettings();

    observer = new Observer(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Initialize textured cubes
    const float cubeColor[] = { 0.5f, 0.5f, 0.5f };
    GLuint woodTexture = BitmapHandler::loadBitmapFromFile("textures/wood.jpg");

    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            Cube* cube = new Cube(0.8f, i * 2.0f, j * 2.0f, 0.0f, cubeColor);
            cube->setTextureForSide(0, woodTexture); // Front
            cube->setTextureForSide(1, woodTexture); // Back
            cube->setTextureForSide(2, woodTexture); // Left
            cube->setTextureForSide(3, woodTexture); // Right
            cube->setTextureForSide(4, woodTexture); // Top
            cube->setTextureForSide(5, woodTexture); // Bottom
            cubes.push_back(cube);
        }
    }

    GLuint wallTexture = BitmapHandler::loadBitmapFromFile("textures/wall.jpg");
    testWall = new Wall(-5.0f, -5.0f, -5.0f, 15.0f, 15.0f, wallTexture);

    glutDisplayFunc(displayCallback);
    glutKeyboardFunc(keyboardCallback);
    glutReshapeFunc(reshapeCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotionCallback);
    glutTimerFunc(1000 / 60, timerCallback, 0);
}

Engine::~Engine() {
    delete observer;
    for (Cube* cube : cubes) {
        delete cube;
    }
    delete testWall;
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

    GLfloat ambientLight[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPosition[] = { 0.0f, 10.0f, 0.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

 
    glEnable(GL_LIGHT1);
    GLfloat light1Diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat light1Specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1Specular);

    glEnable(GL_COLOR_MATERIAL);
}

void Engine::displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadMatrixf(glm::value_ptr(observer->getViewMatrix()));

   
    GLfloat lightPosition[] = { 5.0f, 10.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

  
    GLfloat floorAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat floorDiffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat floorSpecular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat floorShininess = 10.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, floorAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floorDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floorSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, floorShininess);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); 
    glVertex3f(-10.0f, -5.0f, -10.0f);
    glVertex3f(10.0f, -5.0f, -10.0f);
    glVertex3f(10.0f, -5.0f, 10.0f);
    glVertex3f(-10.0f, -5.0f, 10.0f);
    glEnd();

  
    testWall->draw();

    
    glPushMatrix();
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);
    for (Cube* cube : cubes) {
        cube->draw();
    }
    glPopMatrix();

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
    else if (key == 'i') {
        light1Position[1] += 1.0f;
    }
    else if (key == 'k') {
        light1Position[1] -= 1.0f;
    }
    else if (key == 'j') { 
        light1Position[0] -= 1.0f;
    }
    else if (key == 'l') { 
        light1Position[0] += 1.0f;
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
    rotationAngle += 1.0f; // Increment rotation angle
    if (rotationAngle >= 360.0f) {
        rotationAngle -= 360.0f;
    }

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
