#include "Engine.h"
#include "BitmapHandler.h"
#include "Cube.h"
#include "Wall.h"


const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;


int Engine::windowWidth = 800;
int Engine::windowHeight = 600;
Observer* Engine::observer = nullptr;
static bool isMousePressed = false;
static int lastMouseX = -1;
static int lastMouseY = -1;
static GLenum shadingMode = GL_SMOOTH;
static int debugmode;
Observer* observer = nullptr;
Cube* texturedCube = nullptr;
std::vector<Cube*> cubes;
std::vector<Wall*> walls;
float rotationAngle = 0.0f;
Shader* mainShader;
Shader* depthShader;
std::vector<Light> lights;

GLuint texture = 0;

Engine::Engine(int argc, char** argv, int width, int height, const char* title) {
    glutInit(&argc, argv);
    glutInitContextVersion(4, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
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

    setup();

    

    glutDisplayFunc(displayCallback);
    glutKeyboardFunc(keyboardCallback);
    glutReshapeFunc(reshapeCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotionCallback);
    glutTimerFunc(1000 / 60, timerCallback, 0);
}

void Engine::initSettings() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glViewport(0, 0, windowWidth, windowHeight);
    debugmode = 0;
    mainShader = new Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    depthShader = new Shader("shaders/depth_vertex_shader.glsl", "shaders/depth_fragment_shader.glsl");
    initializeLights();
}

void Engine::initializeLights() {

    Light light;
    light.position = glm::vec3(-5.0f, -5.0f, 5.0f);
    light.color = glm::vec3(3.0f, 3.0f, 3.0f);

    glGenFramebuffers(1, &light.shadowFBO);
    glGenTextures(1, &light.shadowMap);
    glBindTexture(GL_TEXTURE_2D, light.shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, light.shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    lights.push_back(light);
}


void Engine::displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    for (size_t i = 0; i < lights.size(); ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, lights[i].shadowFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        depthShader->use();

        glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 100.0f);

        glm::mat4 lightView = glm::lookAt(
            lights[i].position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lights[i].lightSpaceMatrix = lightProjection * lightView;

        glUniformMatrix4fv(glGetUniformLocation(depthShader->getProgramID(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lights[i].lightSpaceMatrix));
        render(depthShader, glm::mat4(1.0f), glm::mat4(1.0f));

        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    mainShader->use();
    glUniform1i(glGetUniformLocation(mainShader->getProgramID(), "debugMode"), debugmode);
    glUniform1i(glGetUniformLocation(mainShader->getProgramID(), "numLights"), lights.size());

    glm::mat4 view = observer->getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    for (size_t i = 0; i < lights.size(); ++i) {
        std::string lightPosUniform = "lights[" + std::to_string(i) + "].position";
        std::string lightColorUniform = "lights[" + std::to_string(i) + "].color";
        std::string lightSpaceMatrixUniform = "lightSpaceMatrix[" + std::to_string(i) + "]";
        std::string shadowMapUniform = "lights[" + std::to_string(i) + "].shadowMap";

        glUniform3fv(glGetUniformLocation(mainShader->getProgramID(), lightPosUniform.c_str()), 1, glm::value_ptr(lights[i].position));
        glUniform3fv(glGetUniformLocation(mainShader->getProgramID(), lightColorUniform.c_str()), 1, glm::value_ptr(lights[i].color));
        glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgramID(), lightSpaceMatrixUniform.c_str()), 1, GL_FALSE, glm::value_ptr(lights[i].lightSpaceMatrix));

        glActiveTexture(GL_TEXTURE6 + i);
        glBindTexture(GL_TEXTURE_2D, lights[i].shadowMap);
        glUniform1i(glGetUniformLocation(mainShader->getProgramID(), shadowMapUniform.c_str()),6+ i);
    }
    render(mainShader, view, projection);

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
    if (key == '1') {
        debugmode =0;
    }
    else if (key == '2') {
        debugmode = 1;
    }
    else if (key == '3') {
        debugmode = 2;
    }
    else if (key == '4') {
        debugmode = 3;
    }
    else if (key == 27) { // ESC
        exit(0);
    }

    keyboard(key, x, y);
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
    const float sensitivity = 0.05f;
    float deltaX = static_cast<float>(x - lastMouseX);
    float deltaY = static_cast<float>(y - lastMouseY);

    float newYaw = observer->getYaw() - deltaX * sensitivity;
    float newPitch = observer->getPitch() + deltaY * sensitivity;

    observer->setYaw(newYaw);
    observer->setPitch(newPitch);

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

    gluPerspective(60.0, static_cast<double>(windowWidth) / windowHeight, 1.0, 100.0);
   
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void Engine::start() {
    glutMainLoop();
}

void Engine::render(Shader * shader, glm::mat4 view,glm::mat4 projection)
{
    for (Wall* wall : walls) {
        wall->draw(mainShader->getProgramID(), glm::mat4(1.0f), view, projection);
    }

    for (Cube* cube : cubes) {
        cube->draw(mainShader->getProgramID(), glm::mat4(1.0f), view, projection);
    }
}

void Engine::setup()
{
    GLuint wallTexture = BitmapHandler::loadBitmapFromFile("textures/wall.jpg");
    GLuint woodTexture = BitmapHandler::loadBitmapFromFile("textures/wood.jpg");

    // Room dimensions
    float roomWidth = 15.0f;
    float roomHeight = 10.0f;
    float roomDepth = 15.0f;

    // Cube properties
    float cubeSize = 1.0f;
    float spacing = 3.0f;

    // Scatter cubes on the floor
    for (int i = -2; i <= 2; ++i) {  // X-axis
        for (int j = -2; j <= 2; ++j) {  // Z-axis
            float x = i * spacing;
            float y = -roomHeight / 2.0f + cubeSize / 2.0f;
            float z = j * spacing;

            Cube* scatteredCube = new Cube(cubeSize, x, y, z, glm::value_ptr(glm::vec3(0.7f, 0.3f, 0.3f)));
            scatteredCube->setTextureForSide(0, woodTexture);
            scatteredCube->setTextureForSide(1, woodTexture);
            scatteredCube->setTextureForSide(2, woodTexture);
            scatteredCube->setTextureForSide(3, woodTexture);
            scatteredCube->setTextureForSide(4, woodTexture);  // Top
            scatteredCube->setTextureForSide(5, woodTexture);
            cubes.push_back(scatteredCube);
        }
    }

    // Floating cube
    Cube* elevatedCube = new Cube(cubeSize, 0.0f, -roomHeight / 2.0f + cubeSize * 5.0f, 0.0f, glm::value_ptr(glm::vec3(0.3f, 0.6f, 0.9f)));
    elevatedCube->setTextureForSide(0, woodTexture);
    elevatedCube->setTextureForSide(1, woodTexture);
    elevatedCube->setTextureForSide(2, woodTexture);
    elevatedCube->setTextureForSide(3, woodTexture);
    elevatedCube->setTextureForSide(4, woodTexture); 
    elevatedCube->setTextureForSide(5, woodTexture);
    cubes.push_back(elevatedCube);

    

    // Vertical center wall
    Wall* centerWall = new Wall(roomDepth, roomHeight, 0.0f, 0.0f, -2.0f, wallTexture);
    walls.push_back(centerWall);

    // Angled walls
    Wall* angledWall1 = new Wall(roomDepth, roomHeight, -5.0f, 0.0f, -3.0f, wallTexture);
    angledWall1->rotateAround(30.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    walls.push_back(angledWall1);

    Wall* angledWall2 = new Wall(roomDepth, roomHeight, 5.0f, 0.0f, 3.0f, wallTexture);
    angledWall2->rotateAround(-30.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    walls.push_back(angledWall2);
}

void Engine::keyboard(unsigned char key, int x, int y)
{
if (key == 'f' || key == 'F') {
        cubes.pop_back();
    }
    else if (key == 'b') {
        glm::vec3 point = observer->getPosition();
        float cubeColor[] = { 0.5f, 0.5f, 0.5f };
        Cube* cube = new Cube(1.0, point.x, point.y, point.z, cubeColor);
        glm::vec3 direction = 3.0f * glm::normalize(observer->getTarget() - point);

        cube->translate(direction);
        cubes.push_back(cube);
        }
}


Engine::~Engine() {
    delete observer;
    for (Cube* cube : cubes) {
        delete cube;
    }

    delete mainShader;
    delete depthShader;

}
