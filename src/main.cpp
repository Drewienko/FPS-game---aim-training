#include "Engine.h"

int main(int argc, char** argv) {
    Engine engine(argc, argv, 800, 600, "FPS-game");
    engine.start();
    return 0;
}