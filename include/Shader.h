// Shader.h
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);
    ~Shader();

    void use() const;
    GLuint getProgramID() const;

private:
    GLuint programID;

    std::string loadShaderFromFile(const std::string& filepath);
    GLuint compileShader(const std::string& source, GLenum type);
};

#endif // SHADER_H