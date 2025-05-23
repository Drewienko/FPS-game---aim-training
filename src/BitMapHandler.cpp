#include "BitmapHandler.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint BitmapHandler::loadBitmapFromFile(const std::string& filename) {
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true); 
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return 0;
    }

    GLenum format = GL_RGB;
    if (channels == 4) {
        format = GL_RGBA;
    }
    else if (channels == 1) {
        format = GL_RED;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    

    glGenerateMipmap(GL_TEXTURE_2D); 

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Loaded texture: " << filename
        << " [ID: " << textureID
        << ", Format: " << (format == GL_RGBA ? "RGBA" : "RGB")
        << ", Size: " << width << "x" << height
        << "]" << std::endl;

    return textureID;
}

GLuint BitmapHandler::createBitmap(int width, int height, unsigned char r, unsigned char g, unsigned char b) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    std::vector<unsigned char> colorData(width * height * 3, 0);
    for (int i = 0; i < width * height; ++i) {
        colorData[i * 3 + 0] = r;
        colorData[i * 3 + 1] = g;
        colorData[i * 3 + 2] = b;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, colorData.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void BitmapHandler::deleteBitmap(GLuint textureID) {
    glDeleteTextures(1, &textureID);
}

void BitmapHandler::copyBitmap(GLuint sourceTextureID, GLuint destinationTextureID, int x, int y, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, sourceTextureID);

    std::vector<unsigned char> pixelData(width * height * 3);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelData.data());

    glBindTexture(GL_TEXTURE_2D, destinationTextureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixelData.data());

    glBindTexture(GL_TEXTURE_2D, 0);
}