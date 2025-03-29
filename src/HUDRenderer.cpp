#include "HUDRenderer.h"
#include "BitmapHandler.h"
#include <iostream>

HUDRenderer::HUDRenderer() {}

HUDRenderer::~HUDRenderer() {
    if (crosshairTexture) {
        BitmapHandler::deleteBitmap(crosshairTexture);
    }
    if (quadVBO) glDeleteBuffers(1, &quadVBO);
    if (quadVAO) glDeleteVertexArrays(1, &quadVAO);
    delete hudShader;
}

void HUDRenderer::init() {
    hudShader = new Shader("shaders/vertex_hud_shader.glsl", "shaders/fragment_hud_shader.glsl");
    loadCrosshairTexture("textures/crosshair.png");
    setupQuad();
}

void HUDRenderer::loadCrosshairTexture(const std::string& path) {
    crosshairTexture = BitmapHandler::loadBitmapFromFile(path);
    if (!crosshairTexture) {
        std::cerr << "Failed to load crosshair texture: " << path << std::endl;
    }
}

void HUDRenderer::setShowCrosshair() {
    this->showCrosshair = !this->showCrosshair;
}

void HUDRenderer::setupQuad() {
    float size = 0.05f;
    float quadVertices[] = {
        // positions   // texCoords
        -size,  size, 0.0f, 1.0f,
        -size, -size, 0.0f, 0.0f,
         size, -size, 1.0f, 0.0f,

        -size,  size, 0.0f, 1.0f,
         size, -size, 1.0f, 0.0f,
         size,  size, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void HUDRenderer::drawCrosshair() {
    if (!showCrosshair || !crosshairTexture || !hudShader) return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    hudShader->use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, crosshairTexture);
    glUniform1i(glGetUniformLocation(hudShader->getProgramID(), "hudTexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
