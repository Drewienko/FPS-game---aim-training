#include "Engine.h"
#include "BitmapHandler.h"
#include "Cube.h"
#include "Wall.h"

//statyczne zmienne do zmiany
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

Wall* testFloor = nullptr;

Wall* testCeiling = nullptr;

GLfloat light1Position[] = { -10.0f, 10.0f, 10.0f, 1.0f };

Engine::Engine(int argc, char** argv, int width, int height, const char* title) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);
    
    initSettings();

    observer = new Observer(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

   
    const float cubeColor[] = { 0.5f, 0.5f, 0.5f };
    GLuint woodTexture = BitmapHandler::loadBitmapFromFile("textures/wood.jpg");

    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            Cube* cube = new Cube(0.8f, i * 2.0f, j * 2.0f, 0.0f, cubeColor);
            cube->setTextureForSide(0, woodTexture); // przod
            cube->setTextureForSide(1, woodTexture); // tyl
            cube->setTextureForSide(2, woodTexture); // lewo
            cube->setTextureForSide(3, woodTexture); // prawo
            cube->setTextureForSide(4, woodTexture); // gora
            cube->setTextureForSide(5, woodTexture); // dol
            cubes.push_back(cube);
        }
    }

    GLuint wallTexture = BitmapHandler::loadBitmapFromFile("textures/wall.jpg");
    testWall = new Wall(-5.0f, -5.0f, -15.0f, 15.0f, 15.0f, wallTexture);
    testWall->rotateAround(90, glm::vec3(1.0f, 0.0f, 0.0f));

    testFloor = new Wall(0.0f, -10.0f, 0.0f, 15.0f, 15.0f, wallTexture);
   testFloor->rotateAround(90, glm::vec3(1.0f, 0.0f, 0.0f));

    //testCeiling = new Wall(0.0f, 15.0f, 0.0f, 15.0f, 15.0f, wallTexture);
    //testCeiling->rotate(90, glm::vec3(1.0f, 1.0f, -1.0f));

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
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    GLfloat lightPosition[] = { 0.0f, 10.0f, 10.0f, 1.0f };
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

void Engine::displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set view and projection matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (isPerspective) {
        gluPerspective(60.0, (double)windowWidth / windowHeight, 0.1, 100.0);
    }
    else {
        glOrtho(-10.0, 10.0, -10.0, 10.0, 0.1, 100.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glm::mat4 view = observer->getViewMatrix();
    glLoadMatrixf(glm::value_ptr(view));

    // Render ground/floor
    glDisable(GL_LIGHTING);
    glColor3f(0.5f, 0.5f, 0.5f); // Light gray floor
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, -1.0f, -10.0f);
    glVertex3f(10.0f, -1.0f, -10.0f);
    glVertex3f(10.0f, -1.0f, 10.0f);
    glVertex3f(-10.0f, -1.0f, 10.0f);
    glEnd();
    glEnable(GL_LIGHTING);

    // Render cubes
    for (Cube* cube : cubes) {
        cube->draw();
    }

    // Render shadows
    GLfloat lightPosition[] = { 0.0f, 10.0f, 10.0f, 1.0f };
    GLfloat shadowMatrix[16] = {
        lightPosition[1], 0, 0, 0,
        -lightPosition[0], 0, -lightPosition[2], -1,
        0, 0, lightPosition[1], 0,
        0, 0, 0, lightPosition[1]
    };

    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 0.2f, 0.2f); // Dark shadow color
    for (Cube* cube : cubes) {
        glPushMatrix();
        glMultMatrixf(shadowMatrix);
        cube->draw();
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}


void Engine::keyboardCallback(unsigned char key, int x, int y) {
    float speed = 0.5f;
    if (key == 'w') {
        observer->moveForward(speed);
    }
    else if (key == 's') {
        observer->moveForward(-speed);
    }
    else if (key == 'a') {
        observer->moveRight(-speed);
    }
    else if (key == 'd') {
        observer->moveRight(speed);
    }
    else if (key == 'q') {
        observer->translate(glm::vec3(0.0f, speed, 0.0f));
    }
    else if (key == 'e') {
        observer->translate(glm::vec3(0.0f, -speed, 0.0f));
    }
    else if (key == 'i') {
        testWall->translate(glm::vec3(0.0f, 0.0f, -speed));
    }
    else if (key == 'k') {
        testWall->translate(glm::vec3(0.0f, 0.0f, speed));
    }
    else if (key == 'j') { 
        testWall->translate(glm::vec3(-speed, 0.0f,  0.0f));
    }
    else if (key == 'l') { 
        testWall->translate(glm::vec3(speed, 0.0f, 0.0f));
    }
    else if (key == 27) { // ESC
        exit(0);
    }
    else if (key == 'f' || key == 'F') {
        shadingMode = GL_FLAT;
        glShadeModel(shadingMode);
        std::cout << "Flat Shading" << std::endl;
    }
    else if (key == 'g' || key == 'G') {
        shadingMode = GL_SMOOTH;
        glShadeModel(shadingMode);
        std::cout << "Gouraud Shading" << std::endl;
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
    rotationAngle += 1.0f; 
    if (rotationAngle >= 360.0f) {
        rotationAngle -= 360.0f;
    }

    

    
    for (int i = 0; i < cubes.size(); i++) {
        glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 point = glm::vec3(i * 0.2f, 0.0f, i * 0.1f);
        cubes[i]->rotatePoint(1.0f, axis, point);
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
