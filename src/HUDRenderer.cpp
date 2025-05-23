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
    //loadCrosshairTexture("textures/crosshair.png");
    setupQuad();
}

void HUDRenderer::loadCrosshairTexture(const std::string& path) {
    crosshairTexture = BitmapHandler::loadBitmapFromFile(path);
    if (!crosshairTexture) {
        std::cerr << "Failed to load crosshair texture: " << path << std::endl;
        return;
    }
    glBindTexture(GL_TEXTURE_2D, crosshairTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void HUDRenderer::setShowCrosshair() {
    this->showCrosshair = !this->showCrosshair;
}

void HUDRenderer::increaseSize() {
    crosshairSize += 1.0f;
    setupQuad();
}

void HUDRenderer::decreaseSize() {
    crosshairSize -= 1.0f;
    setupQuad();
}

void HUDRenderer::setColor(const glm::vec3& color) {
    crosshairColor = color;
}

void HUDRenderer::setupQuad() {
    float quadVertices[] = {
        // positions     // texCoords
        -1.0f,  1.0f,     0.0f, 1.0f,
        -1.0f, -1.0f,     0.0f, 0.0f,
         1.0f, -1.0f,     1.0f, 0.0f,

        -1.0f,  1.0f,     0.0f, 1.0f,
         1.0f, -1.0f,     1.0f, 0.0f,
         1.0f,  1.0f,     1.0f, 1.0f
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


void HUDRenderer::drawCrosshair(int windowWidth, int windowHeight) {
    if (!showCrosshair || !hudShader) return;

    // Ensure HUD is rendered over everything
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    hudShader->use();
    glBindVertexArray(quadVAO);

    glUniform3fv(glGetUniformLocation(hudShader->getProgramID(), "crosshairColor"), 1, glm::value_ptr(crosshairColor));
    float lineWidth = 1.5f;
    float gapSize = 4.0f;

    glUniform1f(glGetUniformLocation(hudShader->getProgramID(), "lineWidth"), lineWidth);
    glUniform1f(glGetUniformLocation(hudShader->getProgramID(), "gapSize"), gapSize);
    glUniform1f(glGetUniformLocation(hudShader->getProgramID(), "crosshairLength"), crosshairSize);
    glUniform2f(glGetUniformLocation(hudShader->getProgramID(), "resolution"),
        static_cast<float>(windowWidth),
        static_cast<float>(windowHeight));

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

