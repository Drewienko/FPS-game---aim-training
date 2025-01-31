#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * @class Shader
 * @brief Klasa obsługująca programy cieniujące w OpenGL.
 *
 * Klasa Shader umożliwia ładowanie, kompilację i używanie programów cieniujących
 * w OpenGL. Obsługuje zarówno podstawowy zestaw (vertex + fragment shader), jak
 * i opcjonalny geometry shader.
 */
class Shader {
public:
    /**
     * @brief Konstruktor ładujący i kompilujący program cieniujący (vertex + fragment).
     *
     * @param vertexPath Ścieżka do pliku z kodem vertex shadera.
     * @param fragmentPath Ścieżka do pliku z kodem fragment shadera.
     */
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @brief Konstruktor ładujący i kompilujący program cieniujący (vertex + fragment + geometry).
     *
     * @param vertexPath Ścieżka do pliku z kodem vertex shadera.
     * @param fragmentPath Ścieżka do pliku z kodem fragment shadera.
     * @param geometryPath Ścieżka do pliku z kodem geometry shadera.
     */
    Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);

    /**
     * @brief Destruktor zwalniający zasoby programu cieniującego.
     */
    ~Shader();

    /**
     * @brief Aktywuje program cieniujący w OpenGL.
     *
     * Wywołanie tej metody powoduje, że OpenGL używa tego programu
     * cieniującego do renderowania.
     */
    void use() const;

    /**
     * @brief Pobiera identyfikator programu OpenGL.
     *
     * @return Identyfikator programu cieniującego.
     */
    GLuint getProgramID() const;

private:
    /**
     * @brief Identyfikator programu cieniującego OpenGL.
     */
    GLuint programID;

    /**
     * @brief Wczytuje kod źródłowy shadera z pliku.
     *
     * @param filepath Ścieżka do pliku z kodem shadera.
     * @return Ciąg znaków zawierający kod źródłowy shadera.
     */
    std::string loadShaderFromFile(const std::string& filepath);

    /**
     * @brief Kompiluje shader na podstawie kodu źródłowego.
     *
     * @param source Kod źródłowy shadera.
     * @param type Typ shadera (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER).
     * @return Identyfikator skompilowanego shadera lub 0 w przypadku błędu.
     */
    GLuint compileShader(const std::string& source, GLenum type);
};

#endif // SHADER_H
