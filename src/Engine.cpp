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
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

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
std::vector<Wall*> walls;
float rotationAngle = 0.0f;
Wall* testWall = nullptr;
Wall* testFloor = nullptr;
Wall* testCeiling = nullptr; 
struct Light {
    glm::vec3 position;
    glm::vec3 color;
    GLuint shadowFBO;
    GLuint shadowMap;
    glm::mat4 lightSpaceMatrix;
};

std::vector<Light> lights;

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

    // Load textures
    GLuint wallTexture = BitmapHandler::loadBitmapFromFile("textures/wall.jpg");
    GLuint woodTexture = BitmapHandler::loadBitmapFromFile("textures/wood.jpg");

    // Room dimensions
    float roomWidth = 10.0f;
    float roomHeight = 10.0f;
    float roomDepth = 10.0f;

    // Cube dimensions
    float cubeSize = 1.0f; // Size of each cube
    float spacing = 2.5f;  // Space between cubes

    // Spawn cubes at varying heights and positions
    for (int i = -1; i <= 1; ++i) {       // X-axis
        for (int j = -1; j <= 1; ++j) {   // Z-axis
            float x = i * spacing;                       // X position
            float y = -roomHeight / 2.0f + cubeSize;     // Y position (just above the floor)
            float z = j * spacing;                       // Z position

            Cube* newCube = new Cube(cubeSize, x, y, z, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
            newCube->setTextureForSide(0, woodTexture); // Front
            newCube->setTextureForSide(1, woodTexture); // Back
            newCube->setTextureForSide(2, woodTexture); // Left
            newCube->setTextureForSide(3, woodTexture); // Right
            newCube->setTextureForSide(4, woodTexture); // Top
            newCube->setTextureForSide(5, woodTexture); // Bottom
            cubes.push_back(newCube);
        }
    }

    

    // Load textures
   // GLuint woodTexture = BitmapHandler::loadBitmapFromFile("textures/wood.jpg");
    //GLuint floorTexture = BitmapHandler::loadBitmapFromFile("textures/floor.jpg");

    // Create a single cube
   

    // Spawn a single elevated cube to test shadow casting
    Cube* elevatedCube = new Cube(cubeSize, 0.0f, -roomHeight / 2.0f + cubeSize * 3.0f, 0.0f, glm::value_ptr(glm::vec3(0.5f, 0.2f, 0.7f)));
    elevatedCube->setTextureForSide(0, woodTexture);
    elevatedCube->setTextureForSide(1, woodTexture);
    elevatedCube->setTextureForSide(2, woodTexture);
    elevatedCube->setTextureForSide(3, woodTexture);
    elevatedCube->setTextureForSide(4, woodTexture);
    elevatedCube->setTextureForSide(5, woodTexture);
    cubes.push_back(elevatedCube);

    // Add a vertical wall in the center to catch shadows
    Wall* shadowWall = new Wall(roomDepth, roomHeight, 0.0f, 0.0f, 0.0f, wallTexture);
    walls.push_back(shadowWall);

    // Add an angled wall for more shadow variation
    Wall* angledWall = new Wall(roomDepth, roomHeight, -3.0f, -1.0f, -3.0f, wallTexture);
    angledWall->rotateAround(45.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate 45 degrees around Y-axis
    walls.push_back(angledWall);


    cubes.clear();
    walls.clear();
    float cubeColor[] = { 0.5f, 0.5f, 0.5f };
    Cube* cube = new Cube(1.0f, 0.0f, 1.0f, -5.0f, cubeColor); // Positioned slightly above the ground
    cube->setTextureForSide(0, woodTexture);
    cube->setTextureForSide(1, woodTexture);
    cube->setTextureForSide(2, woodTexture);
    cube->setTextureForSide(3, woodTexture);
    cube->setTextureForSide(4, woodTexture);
    cube->setTextureForSide(5, woodTexture);
    cubes.push_back(cube);

    // Create a large plane (floor)
    Wall* floor = new Wall(20.0f, 20.0f, 0.0f, -10.0f, 0.0f, wallTexture); // Large floor
    walls.push_back(floor);

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

    debugShader = new Shader("shaders/debug_vertex_shader.glsl", "shaders/debug_fragment_shader.glsl");

    initializeLights();
}

// Initialize lights with positions, colors, and shadow maps
void Engine::initializeLights() {
    for (int i = 0; i < 1; ++i) {
        Light light;
        light.position = (i == 0) ? glm::vec3(10.0f, 0.0f, 15.0f) :
            (i == 1) ? glm::vec3(-3.0f, 6.0f, 3.0f) :
            glm::vec3(1.0f, 3.0f, 2.0f);
        light.color = (i == 0) ? glm::vec3(1.0f, 0.9f, 0.8f) : // Dimmer light
            (i == 1) ? glm::vec3(0.5f, 0.6f, 0.8f) :
            glm::vec3(0.7f, 0.7f, 0.7f);


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
} 

GLuint quadVAO = 0;
GLuint quadVBO = 0;

void setupQuad() {
    float quadVertices[] = {
        // positions        // texCoords
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void renderShadowMapDebug(GLuint shadowMap) {
    setupQuad();
    // Debug function to visualize shadow map
    glViewport(0, 0, 512, 512); // Adjust size as needed
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    debugShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glUniform1i(glGetUniformLocation(debugShader->getProgramID(), "shadowMap"), 0);

    // Render a simple quad to display the shadow map
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Engine::displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    

    // Shadow Pass for Multiple Lights
    for (size_t i = 0; i < lights.size(); ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

        depthShader->use();

        glm::mat4 lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, 1.0f, 100.0f);

        //glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 50.0f);



        glm::mat4 lightView = glm::lookAt(lights[i].position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lights[i].lightSpaceMatrix = lightProjection * lightView;
        std::cout << "Light Space Matrix for Light " << i << ":\n"
            << glm::to_string(lights[i].lightSpaceMatrix) << std::endl;
        glUniformMatrix4fv(glGetUniformLocation(depthShader->getProgramID(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lights[i].lightSpaceMatrix));
        glCullFace(GL_FRONT);
        // Render all objects
        for (Cube* cube : cubes) {
            glm::mat4 model = glm::mat4(1.0f);
            cube->draw(depthShader->getProgramID(), model, glm::mat4(1.0f), glm::mat4(1.0f));
        }
        for (Wall* wall : walls) {
            glm::mat4 model = glm::mat4(1.0f);
            wall->draw(depthShader->getProgramID(), model, glm::mat4(1.0f), glm::mat4(1.0f));
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Main Render Pass
    mainShader->use();
    glUniform1i(glGetUniformLocation(mainShader->getProgramID(), "numLights"), lights.size());

    glm::mat4 view = observer->getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    for (size_t i = 0; i < lights.size(); ++i) {
        std::string lightPosUniform = "lights[" + std::to_string(i) + "].position";
        std::string lightColorUniform = "lights[" + std::to_string(i) + "].color";
        std::string lightSpaceMatrixUniform = "lights[" + std::to_string(i) + "].lightSpaceMatrix";
        std::string shadowMapUniform = "lights[" + std::to_string(i) + "].shadowMap";

        glUniform3fv(glGetUniformLocation(mainShader->getProgramID(), lightPosUniform.c_str()), 1, glm::value_ptr(lights[i].position));
        glUniform3fv(glGetUniformLocation(mainShader->getProgramID(), lightColorUniform.c_str()), 1, glm::value_ptr(lights[i].color));
        glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgramID(), lightSpaceMatrixUniform.c_str()), 1, GL_FALSE, glm::value_ptr(lights[i].lightSpaceMatrix));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lights[i].shadowMap);
        glUniform1i(glGetUniformLocation(mainShader->getProgramID(), shadowMapUniform.c_str()), 1 + i);
    }
    glCullFace(GL_BACK);
    // Render scene objects
    for (Cube* cube : cubes) {
        glm::mat4 model = glm::mat4(1.0f);
        cube->draw(mainShader->getProgramID(), model, view, projection);
    }
    for (Wall* wall : walls) {
        glm::mat4 model = glm::mat4(1.0f);
        wall->draw(mainShader->getProgramID(), model, view, projection);
    }
    if(shadingMode == GL_FLAT)
         renderShadowMapDebug(lights[0].shadowMap);

    glutSwapBuffers();
}



void Engine::keyboardCallback(unsigned char key, int x, int y) {
    float speed = 0.5f; // Movement speed for observer and light

    // Observer movement
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

    

    // Shading modes
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

    // Spawn a cube in front of the observer
    else if (key == 'b') {
        glm::vec3 point = observer->getPosition();
        float cubeColor[] = { 0.5f, 0.5f, 0.5f };
        Cube* cube = new Cube(1.0, point.x, point.y, point.z, cubeColor);
        glm::vec3 direction = 3.0f * glm::normalize(observer->getTarget() - point);

        cube->translate(direction);
        cubes.push_back(cube);
    }
    std::cout << glm::to_string(observer->getPosition()) << std::endl;
    // Trigger redraw
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


   // lightPos.x = 5.0f * sin(value);
    //lightPos.z = 5.0f * cos(value);

    for (Cube* cube : cubes) {
       //cube->translate(glm::vec3(0.0f, 0.0f, 0.0f));
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
