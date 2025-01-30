#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <GL/glew.h>
#include "Shader.h"
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Observer.h"
#include "Cube.h"
#include "BitMapHandler.h"


struct Light {
    glm::vec3 position;
    glm::vec3 color;
    GLuint shadowFBO;
    GLuint shadowMap;
    glm::mat4 lightSpaceMatrix;
};

class Engine {
public:
    Engine(int argc, char** argv, int width, int height, const char* title);
    ~Engine();

    void start();

    static void setup();
    static void keyboard(unsigned char key, int x, int y);

    static bool isPerspective;
    static int windowWidth;
    static int windowHeight;
    static Observer* observer;
private:
    void initializeLights();
    void initSettings();
    static void displayCallback();
    
    static void keyboardCallback(unsigned char key, int x, int y);
    static void reshapeCallback(int w, int h);
    static void mouseCallback(int button, int state, int x, int y);
    static void mouseMotionCallback(int x, int y);
    static void timerCallback(int value);
    static void updateProjectionMatrix();
};

#endif // ENGINE_H
