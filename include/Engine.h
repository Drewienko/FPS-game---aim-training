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

/**
 * @struct Light
 * @brief Struktura reprezentująca źródło światła w scenie.
 *
 * Przechowuje pozycję, kolor oraz mapę cieni potrzebną do obliczania efektów oświetlenia.
 */
struct Light {
    glm::vec3 position;      /**< Pozycja światła w przestrzeni 3D. */
    glm::vec3 color;         /**< Kolor światła. */
    GLuint shadowFBO;        /**< Identyfikator FBO dla shadow mappingu. */
    GLuint shadowMap;        /**< Identyfikator tekstury cieni. */
    glm::mat4 lightSpaceMatrix; /**< Macierz przestrzeni światła do rzutowania cieni. */
};

/**
 * @class Engine
 * @brief Klasa głównego silnika renderującego.
 *
 * Odpowiada za inicjalizację, obsługę zdarzeń oraz główną pętlę renderowania.
 */
class Engine {
public:
    /**
     * @brief Konstruktor inicjalizujący silnik z parametrami okna.
     *
     * @param argc Liczba argumentów wiersza poleceń.
     * @param argv Tablica argumentów wiersza poleceń.
     * @param width Szerokość okna renderowania.
     * @param height Wysokość okna renderowania.
     * @param title Tytuł okna aplikacji.
     */
    Engine(int argc, char** argv, int width, int height, const char* title);

    /**
     * @brief Destruktor silnika.
     */
    ~Engine();

    /**
     * @brief Uruchamia pętlę główną silnika.
     */
    void start();

    /**
     * @brief Funkcja inicjalizująca OpenGL i ustawienia silnika.
     */
    static void setup();

    /**
     * @brief Obsługuje zdarzenia klawiatury.
     *
     * @param key Wciśnięty klawisz.
     * @param x Pozycja kursora myszy w osi X.
     * @param y Pozycja kursora myszy w osi Y.
     */
    static void keyboard(unsigned char key, int x, int y);

    /**
     * @brief Określa, czy kamera renderuje w trybie perspektywicznym.
     */
    static bool isPerspective;

    /**
     * @brief Aktualna szerokość okna renderowania.
     */
    static int windowWidth;

    /**
     * @brief Aktualna wysokość okna renderowania.
     */
    static int windowHeight;


private:
    /**
     * @brief Inicjalizuje źródła światła w scenie.
     */
    void initializeLights();

    /**
     * @brief Ustawia początkowe opcje renderowania.
     */
    void initSettings();

    /**
     * @brief Funkcja renderowania sceny, wywoływana w pętli głównej.
     */
    static void displayCallback();

    /**
     * @brief Obsługa zdarzeń klawiatury (przekierowanie do `keyboard`).
     *
     * @param key Wciśnięty klawisz.
     * @param x Pozycja kursora myszy w osi X.
     * @param y Pozycja kursora myszy w osi Y.
     */
    static void keyboardCallback(unsigned char key, int x, int y);

    /**
     * @brief Obsługa zmiany rozmiaru okna.
     *
     * @param w Nowa szerokość okna.
     * @param h Nowa wysokość okna.
     */
    static void reshapeCallback(int w, int h);

    /**
     * @brief Obsługa zdarzeń myszy.
     *
     * @param button Wciśnięty przycisk myszy.
     * @param state Stan przycisku (wciśnięty/zwolniony).
     * @param x Pozycja kursora w osi X.
     * @param y Pozycja kursora w osi Y.
     */
    static void mouseCallback(int button, int state, int x, int y);

    /**
     * @brief Obsługa ruchu myszy.
     *
     * @param x Aktualna pozycja kursora w osi X.
     * @param y Aktualna pozycja kursora w osi Y.
     */
    static void mouseMotionCallback(int x, int y);

    /**
     * @brief Funkcja timerowa do aktualizacji sceny w określonych odstępach czasu.
     *
     * @param value Wartość licznika czasu.
     */
    static void timerCallback(int value);

    /**
     * @brief Aktualizuje macierz projekcji po zmianie trybu widoku.
     */
    static void updateProjectionMatrix();
};

#endif // ENGINE_H
