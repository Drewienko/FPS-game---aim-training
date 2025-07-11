#include "Engine.h"


const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;


int Engine::windowWidth = 800;
int Engine::windowHeight = 600;
static bool isMousePressed = false;
static int lastMouseX = -1;
static int lastMouseY = -1;
static int debugmode;
Observer* observer = nullptr;
std::vector<Cube*> cubes;
std::vector<Wall*> walls;
std::vector<ModelObject*> drawableObjects;
Shader* mainShader;
Shader* depthShader;
std::vector<Light> lights;
HUDRenderer hud;
GLuint wallTexture = 0;
GLuint woodTexture = 0;
Cube* lightCube = nullptr;
HeldWeapon* currentWeapon = nullptr;


std::set<char> currentlyHeldKeys;
float lastFrameTime = 0.0f;

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

    observer = new Observer(glm::vec3(0.0f, 1.0f, -6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    wallTexture = BitmapHandler::loadBitmapFromFile("textures/wall.jpg");
    woodTexture = BitmapHandler::loadBitmapFromFile("textures/wood.jpg");
    hud.init();

    setup2();

    

    glutDisplayFunc(displayCallback);
    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
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

    glm::vec3 lightPositions[] = {
    glm::vec3(-5.0f, 1.0f, -9.0f),
    glm::vec3(5.0f, 2.0f, -8.0f),
    glm::vec3(1.0f, 10.0f, 10.0f)
    };

    for (int i = 0; i < lightPositions->length(); i++) {
        Light light;
        light.position = lightPositions[i];
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
    float color[] = { 0.2,0.8,0.8 };
    GLuint texture = BitmapHandler::createBitmap(1024, 1024, 255*color[0], 255 * color[1], 255 * color[2]);
    lightCube = new Cube(0.5, 0.0, 0.0, 0.0, texture);

}


void Engine::displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    for (size_t i = 0; i < lights.size(); i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, lights[i].shadowFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        depthShader->use();

        glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 100.0f);

        glm::mat4 lightView = glm::lookAt(
            lights[i].position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lights[i].lightSpaceMatrix = lightProjection * lightView;

        glUniformMatrix4fv(glGetUniformLocation(depthShader->getProgramID(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lights[i].lightSpaceMatrix));
        glDisable(GL_CULL_FACE);
        for (Wall* wall : walls) {
            glm::mat4 model = glm::mat4(1.0f);
            wall->draw(depthShader->getProgramID(), model, glm::mat4(1.0f), glm::mat4(1.0f));
        }
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        for (Cube* cube : cubes) {
            glm::mat4 model = glm::mat4(1.0f);
            cube->draw(depthShader->getProgramID(), model, glm::mat4(1.0f), glm::mat4(1.0f));
        }

        for (ModelObject* model : drawableObjects) {
            glm::mat4 modelMatrix = model->getModelMatrix();
            model->draw(depthShader->getProgramID(), modelMatrix, glm::mat4(1.0f), glm::mat4(1.0f));
        }

        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
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
       
        glActiveTexture(GL_TEXTURE2 + i);
        glBindTexture(GL_TEXTURE_2D, lights[i].shadowMap);
        glUniform1i(glGetUniformLocation(mainShader->getProgramID(), shadowMapUniform.c_str()),2+ i);
    }
    glDisable(GL_CULL_FACE);
    for (Wall* wall : walls) {
        wall->draw(mainShader->getProgramID(), glm::mat4(1.0f), view, projection);
    }
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    for (Cube* cube : cubes) {
        cube->draw(mainShader->getProgramID(), glm::mat4(1.0f), view, projection);
    }
    for (ModelObject* model : drawableObjects) {
            glm::mat4 modelMatrix = model->getModelMatrix();
            model->draw(mainShader->getProgramID(), modelMatrix, view, projection);
            //std::cout << "Drawing model" << std::endl;
    }
    

    for (size_t i = 0; i < lights.size(); i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lights[i].position);
        glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
        lightCube->draw(mainShader->getProgramID(), model, view, projection);
    }

    if (currentWeapon) {
        glm::mat4 model = currentWeapon->getModelMatrix();
        glm::mat4 weaponView = glm::mat4(1.0f); // bro� zawsze patrzy wprost
        glm::mat4 weaponProjection = glm::perspective(glm::radians(60.0f),
            (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

        currentWeapon->draw(mainShader->getProgramID(), model, weaponView, weaponProjection);
    }

    hud.drawCrosshair(windowWidth, windowHeight);

    glutSwapBuffers();
}

void Engine::keyboardUpCallback(unsigned char key, int x, int y) {
    currentlyHeldKeys.erase(key);
}


void Engine::keyboardCallback(unsigned char key, int x, int y) {
    float speed = 0.5f; 
    switch (key) {
    case ' ':
    case 'w':
    case 's':
    case 'a':
    case 'd':
        currentlyHeldKeys.insert(key);
        break;
    case 'q':
        observer->translate(glm::vec3(0.0f, speed, 0.0f));
        break;
    case 'e':
        observer->translate(glm::vec3(0.0f, -speed, 0.0f));
        break;
    case '1':
        debugmode = 0;
        break;
    case '2':
        debugmode = 1;
        break;
    case '3':
        debugmode = 2;
        break;
    case '4':
        debugmode = 3;
        break;
    case 27: // ESC

        exit(0);
        break;
    case 'm': // symulacja strza�u
        if (currentWeapon) {
            currentWeapon->triggerRecoil();
        }
        glm::vec3 rayOrigin = observer->getPosition();
        glm::vec3 rayDir = glm::normalize(observer->getTarget() - rayOrigin);

        for (auto it = drawableObjects.begin(); it != drawableObjects.end(); ) {//bez it++
            if (auto* target = dynamic_cast<TargetObject*>(*it)) {
                if (target->isHitByRay(rayOrigin, rayDir)) {
                    target->onHit();
                    it = drawableObjects.erase(it);//zwraca iterator drawableObjects.end() albo nastepny iterator
                    continue;
                }
            }
            it++;
        }
        break;
    case '`': observer->setPosition(glm::vec3(0.0f, 3.0f, 0.0f)); break;
    case 'i': currentWeapon->translate(glm::vec3(0.0f, 0.01f, 0.0f)); break; // up
    case 'k': currentWeapon->translate(glm::vec3(0.0f, -0.01f, 0.0f)); break; // down
    case 'j': currentWeapon->translate(glm::vec3(-0.01f, 0.0f, 0.0f)); break; // left
    case 'l': currentWeapon->translate(glm::vec3(0.01f, 0.0f, 0.0f)); break; // right
    case 'u': currentWeapon->translate(glm::vec3(0.0f, 0.0f, -0.01f)); break; // forward
    case 'o': currentWeapon->translate(glm::vec3(0.0f, 0.0f, 0.01f)); break;  // back
    case 'f':
    case 'F':
        if (!cubes.empty()) {
            cubes.pop_back();
        }
        break;

    case '+':
    case '=':
        hud.increaseSize();
        break;
    case '-':
        hud.decreaseSize();
        break;
    case 'r':
        hud.setColor(glm::vec3(1.0f, 0.0f, 0.0f)); // czerwony
        break;
    case 'g':
        hud.setColor(glm::vec3(0.0f, 1.0f, 0.0f)); // zielony
        break;
    case 'y':
        hud.setColor(glm::vec3(0.0f, 0.5f, 1.0f)); // niebieski
        break;

    case 'b': {
        glm::vec3 point = observer->getPosition();
        Cube* cube = new Cube(1.0, point.x, point.y, point.z, woodTexture);
        glm::vec3 direction = 3.0f * glm::normalize(observer->getTarget() - point);

        cube->translate(direction);

        cubes.push_back(cube);
        break;
    }
    case 'p': 
        glutSetCursor(GLUT_CURSOR_NONE);
        break;
    case '[': 
        glutSetCursor(GLUT_CURSOR_INHERIT);  
        break;
    case 'h':
        hud.setShowCrosshair();
        break;

    default:
        break;
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
    const float sensitivity = 0.05f;
    float deltaX = static_cast<float>(x - lastMouseX);
    float deltaY = static_cast<float>(y - lastMouseY);

    float newYaw = observer->getYaw() + deltaX * sensitivity;
    float newPitch = observer->getPitch() - deltaY * sensitivity;

    observer->setYaw(newYaw);
    observer->setPitch(newPitch);

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}

void Engine::reshapeCallback(int w, int h) {
    windowHeight = h;
    windowWidth = w;
    updateProjectionMatrix();
}

void Engine::timerCallback(int value) {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    observer->applyMovementInput(currentlyHeldKeys, deltaTime, walls);
    observer->updatePhysics(deltaTime, walls);

    currentWeapon->update(deltaTime);

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


void Engine::setup()
{
    ModelObject* model = new ModelObject("models/P90.obj");
    model->setPosition(glm::vec3(0.0f, 1.0f, -0.5f));
    model->setScale(glm::vec3(0.5f));
    model->rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    drawableObjects.push_back(model);

    model = new ModelObject("models/AK-47.obj");
    model->setPosition(glm::vec3(0.0f, 2.0f, -0.3f));
    model->setScale(glm::vec3(0.5f));
    model->rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    drawableObjects.push_back(model);

    model = new ModelObject("models/Pistol.obj");
    model->setPosition(glm::vec3(0.0f, 3.0f, -0.5f));
    model->setScale(glm::vec3(0.1f));
    model->rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    drawableObjects.push_back(model);

    model = new ModelObject("models/SniperRifle.obj");
    model->setPosition(glm::vec3(0.0f, 4.0f, -0.3f));
    model->setScale(glm::vec3(0.5f));
    model->rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    drawableObjects.push_back(model);

    TargetObject* target;
    for (int i = 0; i < 5; i++) {
        target = new TargetObject("models/target.obj");
        target->setPosition(glm::vec3(1.0f*i, 0.0f, 2.0f));
        drawableObjects.push_back(target);
    }


    float roomWidth = 15.0f;
    float roomHeight = 16.0f;
    float roomDepth = 14.0f;


    Wall* centerWall = new Wall(roomDepth, roomHeight, 0.0f, 0.0f, -2.0f, wallTexture);
    walls.push_back(centerWall);

    Wall* angledWall1 = new Wall(roomDepth, roomHeight, -5.0f, 0.0f, -3.0f, wallTexture);
    angledWall1->rotateAround(30.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    walls.push_back(angledWall1);

    Wall* angledWall2 = new Wall(roomDepth, roomHeight, 5.0f, 0.0f, 3.0f, wallTexture);
    angledWall2->rotateAround(-30.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    walls.push_back(angledWall2);
}

void Engine::setup2()
{
    float roomWidth = 15.0f;
    float roomDepth = 30.0f;
    float roomHeight = 5.0f;

    Wall* floor = new Wall(roomWidth, roomDepth, -7.5f, -15.5f, 5.5f, woodTexture);
    floor->rotateAround(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    walls.push_back(floor);

    Wall* leftWall = new Wall(roomDepth, roomHeight, -roomWidth / 2.0f, 0.0f, 0.0f, wallTexture);
    leftWall->rotateAround(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    leftWall->translate(glm::vec3(-15.0f, -0.5f, 5.5f));
    walls.push_back(leftWall);

    Wall* rightWall = new Wall(roomDepth, roomHeight, roomWidth / 2.0f, 0.0f, 0.0f, wallTexture);
    rightWall->rotateAround(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    rightWall->translate(glm::vec3(-15.0f, -0.5f, 5.5f));
    walls.push_back(rightWall);

    Wall* backWall = new Wall(roomWidth, roomHeight, 0.0f, 0.0f, roomDepth / 2.0f, wallTexture);
    walls.push_back(backWall);
    backWall->translate(glm::vec3(-7.5f, -0.5f, 5.5f));


    for (int i = -2; i <= 2; ++i) {
        ModelObject* table = new ModelObject("models/table.obj");
        table->setPosition(glm::vec3(i * 2.5f, 0.0f, -1.5f));
        table->setScale(glm::vec3(0.01f));
        drawableObjects.push_back(table);
    }

    ModelObject* ak = new ModelObject("models/AK-47.obj");
    ak->setPosition(glm::vec3(-6.0f, 1.0f, -1.0f));
    ak->setScale(glm::vec3(0.5f));
    ak->rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    drawableObjects.push_back(ak);

    ModelObject* sniper = new ModelObject("models/SniperRifle.obj");
    sniper->setPosition(glm::vec3(-6.0f, 2.0f, -1.0f));
    sniper->setScale(glm::vec3(0.5f));
    sniper->rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    drawableObjects.push_back(sniper);

    for (int i = 0; i < 5; ++i) {
        TargetObject* target = new TargetObject("models/Human.obj");
        target->setPosition(glm::vec3(i * 2.5f - 5.0f, 0.0f, 20.0f));

        target->setScale(glm::vec3(0.5f));
        target->rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        drawableObjects.push_back(target);
    }

    currentWeapon = new HeldWeapon("models/P90.obj");
    currentWeapon->setScale(glm::vec3(0.5f));
    //currentWeapon->rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

}


void Engine::keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    

    default:
        break;
    }
}


Engine::~Engine() {
    delete observer;
    for (Cube* cube : cubes) {
        delete cube;
    }

    for (Wall* wall : walls) {
        delete wall;
    }
    BitmapHandler::deleteBitmap(wallTexture);
    BitmapHandler::deleteBitmap(wallTexture);

    for (Light light : lights) {
        BitmapHandler::deleteBitmap(light.shadowFBO);
    }
    

    delete mainShader;
    delete depthShader;

}
