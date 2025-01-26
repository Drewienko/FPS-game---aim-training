#include "Engine.h"
#include "BitmapHandler.h"
#include "Cube.h"
#include "Wall.h"

// Shaders
Shader* mainShader = nullptr;
Shader* depthShader = nullptr;
Shader* debugShader = nullptr;

// Shadow mapping
GLuint depthMapFBO;
GLuint depthMap;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

// Static variables
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
GLuint texture = 0;

Engine::Engine(int argc, char** argv, int width, int height, const char* title) {
    glutInit(&argc, argv);
    glutInitContextVersion(4, 6); // Request OpenGL 4.6
    glutInitContextProfile(GLUT_CORE_PROFILE); // Force core profile
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);
    if (glGetString(GL_VERSION) == nullptr) {
        std::cerr << "OpenGL context creation failed!" << std::endl;
    }
    else {
        std::cout << "OpenGL context created successfully." << std::endl;
    }
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW Initialization failed: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    initSettings();

    observer = new Observer(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    const float cubeColor[] = { 0.5f, 0.5f, 0.5f };
    GLuint woodTexture = BitmapHandler::loadBitmapFromFile("textures/wood.jpg");
    texture = woodTexture;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            Cube* cube = new Cube(0.8f, i * 2.0f, j * 2.0f, 0.0f, cubeColor);
            cube->setTextureForSide(0, woodTexture);
            cube->setTextureForSide(1, woodTexture);
            cube->setTextureForSide(2, woodTexture);
            cube->setTextureForSide(3, woodTexture);
            cube->setTextureForSide(4, woodTexture);
            cube->setTextureForSide(5, woodTexture);
            cubes.push_back(cube);
        }
    }

    GLuint wallTexture = BitmapHandler::loadBitmapFromFile("textures/wall.jpg");
    testWall = new Wall(-5.0f, -5.0f, -15.0f, 15.0f, 15.0f, wallTexture);
    testWall->rotateAround(90, glm::vec3(1.0f, 0.0f, 0.0f));

    testFloor = new Wall(0.0f, -10.0f, 0.0f, 15.0f, 15.0f, wallTexture);
    testFloor->rotateAround(90, glm::vec3(1.0f, 0.0f, 0.0f));

    glutDisplayFunc(displayCallback);
    glutKeyboardFunc(keyboardCallback);
    glutReshapeFunc(reshapeCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotionCallback);
    glutTimerFunc(1000 / 60, timerCallback, 0);
}

void Engine::initSettings() {
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Set clear color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Set viewport
    glViewport(0, 0, windowWidth, windowHeight);

    // Debug OpenGL state
    GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
    GLboolean cullFace = glIsEnabled(GL_CULL_FACE);

    std::cout << "Depth Test: " << (depthTest ? "Enabled" : "Disabled")
        << ", Cull Face: " << (cullFace ? "Enabled" : "Disabled") << std::endl;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    std::cout << "Viewport: x=" << viewport[0]
        << ", y=" << viewport[1]
        << ", width=" << viewport[2]
        << ", height=" << viewport[3] << std::endl;

    mainShader = new Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    depthShader = new Shader("shaders/depth_vertex_shader.glsl", "shaders/depth_fragment_shader.glsl");

}

void Engine::displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Shadow pass
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    depthShader->use();

    // Set the light space matrix
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
    glm::mat4 lightView = glm::lookAt(glm::vec3(2.0f, 4.0f, -5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    glUniformMatrix4fv(glGetUniformLocation(depthShader->getProgramID(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    // Render the cubes for the shadow map
    for (Cube* cube : cubes) {
        glm::mat4 model = glm::mat4(1.0f); // Reset model matrix
        cube->draw(depthShader->getProgramID(), model, glm::mat4(1.0f), glm::mat4(1.0f));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Normal pass
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mainShader->use();

    // Uniform locations
    GLint lightPosLoc = glGetUniformLocation(mainShader->getProgramID(), "lightPos");
    GLint viewPosLoc = glGetUniformLocation(mainShader->getProgramID(), "viewPos");
    GLint lightSpaceMatrixLoc = glGetUniformLocation(mainShader->getProgramID(), "lightSpaceMatrix");
    GLint shadowMapLoc = glGetUniformLocation(mainShader->getProgramID(), "shadowMap");

    // Set light and camera uniforms
    glm::vec3 lightPos = glm::vec3(2.0f, 4.0f, -5.0f); // Example light position
    glm::vec3 viewPos = observer->getPosition();       // Camera position

    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));
    glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    // Bind shadow map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(shadowMapLoc, 1);

    // Set model, view, projection matrices (existing code)
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = observer->getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Render the cubes
    for (Cube* cube : cubes) {
        cube->draw(mainShader->getProgramID(), model, view, projection);
    }

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
        Cube* cube = new Cube(1.0, point.x, point.y, point.z, cubeColor);
        glm::vec3 direction = 3.0f * glm::normalize(observer->getTarget() - point);

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




    for (Cube* cube : cubes) {
       // cube->translate(glm::vec3(1.0f, 0.0f, 0.0f));
        //cube->rotate(1, glm::vec3(1.0f, 0.0f, 0.0f));
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

Engine::~Engine() {
    delete observer;
    for (Cube* cube : cubes) {
        delete cube;
    }
    delete testWall;
    delete testFloor;

    delete mainShader;
    delete depthShader;
    delete debugShader;

    glDeleteFramebuffers(1, &depthMapFBO);
    glDeleteTextures(1, &depthMap);
}
