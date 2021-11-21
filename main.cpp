#include "engine/Engine.h"

int main(int argc, char** argv) {
    Engine engine = Engine();
    if (!engine.start("^_^", 1024, 720)) {
        printf("Error starting 3D engine\n");
    } else {
        printf("Ended start engine task\n");
    }
    return 0;
}
