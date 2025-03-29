#ifndef HUD_RENDERER_H
#define HUD_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include "Shader.h"

class HUDRenderer {
public:
    HUDRenderer();
    ~HUDRenderer();

    void init();
    void drawCrosshair();
    void setShowCrosshair();

private:
    GLuint crosshairTexture = 0;
    bool showCrosshair = true;

    GLuint quadVAO = 0, quadVBO = 0;
    Shader* hudShader = nullptr;

    void loadCrosshairTexture(const std::string& path);
    void setupQuad();
};

#endif // HUD_RENDERER_H
