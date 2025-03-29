#ifndef BITMAPHANDLER_H
#define BITMAPHANDLER_H

#include <GL/glew.h>
#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <string>

/**
 * @class BitmapHandler
 * @brief Klasa narzędziowa do zarządzania teksturami bitmapowymi w OpenGL.
 *
 * Klasa `BitmapHandler` zawiera metody statyczne do ładowania, tworzenia,
 * kopiowania oraz usuwania tekstur bitmapowych w OpenGL.
 */
class BitmapHandler {
public:

    /**
     * @brief Ładuje bitmapę z pliku i tworzy teksturę OpenGL.
     *
     * Funkcja wczytuje plik bitmapy o podanej nazwie i generuje odpowiadającą
     * jej teksturę OpenGL.
     *
     * @param filename Ścieżka do pliku bitmapy.
     * @return Identyfikator tekstury OpenGL lub 0 w przypadku błędu.
     */
    static GLuint loadBitmapFromFile(const std::string& filename);

    /**
     * @brief Tworzy jednolitą bitmapę o podanym kolorze i rozmiarze.
     *
     * Funkcja generuje teksturę o zadanych wymiarach i wypełnia ją
     * jednolitym kolorem.
     *
     * @param width Szerokość bitmapy w pikselach.
     * @param height Wysokość bitmapy w pikselach.
     * @param r Wartość czerwieni (0-255).
     * @param g Wartość zieleni (0-255).
     * @param b Wartość niebieskiego (0-255).
     * @return Identyfikator utworzonej tekstury OpenGL.
     */
    static GLuint createBitmap(int width, int height, unsigned char r, unsigned char g, unsigned char b);

    /**
     * @brief Usuwa teksturę OpenGL.
     *
     * Funkcja zwalnia pamięć zajmowaną przez teksturę OpenGL o podanym identyfikatorze.
     *
     * @param textureID Identyfikator tekstury OpenGL do usunięcia.
     */
    static void deleteBitmap(GLuint textureID);

    /**
     * @brief Kopiuje fragment jednej bitmapy do innej.
     *
     * Funkcja kopiuje wycinek jednej tekstury do drugiej, umożliwiając operacje
     * edycji obrazu w pamięci GPU.
     *
     * @param sourceTextureID Identyfikator źródłowej tekstury OpenGL.
     * @param destinationTextureID Identyfikator docelowej tekstury OpenGL.
     * @param x Pozycja X (lewy górny róg) kopiowanego fragmentu.
     * @param y Pozycja Y (lewy górny róg) kopiowanego fragmentu.
     * @param width Szerokość kopiowanego obszaru.
     * @param height Wysokość kopiowanego obszaru.
     */
    static void copyBitmap(GLuint sourceTextureID, GLuint destinationTextureID, int x, int y, int width, int height);
};

#endif // BITMAPHANDLER_H
