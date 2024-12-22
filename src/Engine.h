#ifndef ENGINE_H
#define ENGINE_H

#include "PrimitiveRenderer.h"
#include "Observer.h"
#include "Cube.h"
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Engine {
public:
    Engine(int argc, char** argv, int width, int height, const char* title);
    ~Engine();

    void start();
    static void setClearColor(float r, float g, float b, float a = 1.0f);

private:
    static bool isPerspective;
    static int windowWidth;
    static int windowHeight;
    static Observer* observer;

    void initSettings();
    void initLighting();
    static void displayCallback();
    static void keyboardCallback(unsigned char key, int x, int y);
    static void reshapeCallback(int w, int h);
    static void mouseCallback(int button, int state, int x, int y);
    static void mouseMotionCallback(int x, int y);
    static void timerCallback(int value);
    static void updateProjectionMatrix();
};

#endif // ENGINE_H
