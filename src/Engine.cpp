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
    GLfloat light1Position[] = { 2.0f, 5.0f, 2.0f, 1.0f };
    GLfloat light1Diffuse[] = { 1.0f, 0.8f, 0.6f, 1.0f };
    GLfloat light1Specular[] = { 1.0f, 0.9f, 0.7f, 1.0f };
    GLfloat spotDirection[] = { 0.0f, -1.0f, 0.0f };

    glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1Specular);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0f);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10.0f);

    glEnable(GL_COLOR_MATERIAL);
}

void Engine::displayCallback() {
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadMatrixf(glm::value_ptr(observer->getViewMatrix()));


    GLfloat lightPosition[] = { 5.0f, 10.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    GLfloat light1Position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light1Position);

    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(0.0f, 10.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 0.8f);
    glutSolidSphere(0.2f, 16, 16);
    glPopMatrix();


    GLfloat floorAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat floorDiffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat floorSpecular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat floorShininess = 10.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, floorAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floorDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floorSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, floorShininess);


    testWall->draw();
    testFloor->draw();

    //testCeiling->draw();

    //glPushMatrix();
   // glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);
    for (Cube* cube : cubes) {
        cube->draw();
    }
    // glPopMatrix();
    setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
        testWall->translate(glm::vec3(-speed, 0.0f, 0.0f));
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
    else if (key == 'b') {
        glm::vec3 point = observer->getPosition();
        float cubeColor[] = { 0.5f, 0.5f, 0.5f };
        Cube *cube = new Cube(1.0, point.x, point.y, point.z, cubeColor);
        glm::vec3 direction = 3.0f * glm::normalize(observer->getTarget()-point);

        cube->translate(direction);
        cubes.push_back(cube);

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
    glm::vec3 point = observer->getPosition();
    observer->translate(-point);
    observer->rotate(deltaX * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
    observer->rotate(deltaY * 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
    observer->translate(point);
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
        //cubes[i]->rotatePoint(1.0f, axis, point);
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
