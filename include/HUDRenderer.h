#ifndef HUD_RENDERER_H
#define HUD_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "Shader.h"

class HUDRenderer {
public:
    HUDRenderer();
    ~HUDRenderer();

    void init();
    void drawCrosshair(int windowWidth,int windowHeight);
    void setShowCrosshair();
    void increaseSize();
    void decreaseSize();
    void setColor(const glm::vec3& color);

private:
    GLuint crosshairTexture = 0;
    bool showCrosshair = true;
    float crosshairSize = 10.0f;
    glm::vec3 crosshairColor = glm::vec3(1.0f, 1.0f, 1.0f); // bia³y domyœlnie

    GLuint quadVAO = 0, quadVBO = 0;
    Shader* hudShader = nullptr;

    void loadCrosshairTexture(const std::string& path);
    void setupQuad();
};

#endif // HUD_RENDERER_H
