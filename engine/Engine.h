//
// Created by iXasthur on 15.11.2020.
//

#ifndef V_3D_ENGINE_H
#define V_3D_ENGINE_H

#include <array>
#include <SDL2/SDL.h>
#include "scene/Scene.h"
#include "utils/Matrix4.h"
#include <cmath>

class Engine {
private:
    SDL_Window *window = nullptr;

    Scene scene = Scene();

    int renderWidth = -1;
    int renderHeight = -1;

    Matrix4 matProj = Matrix4();

    void startRenderLoop() {
        bool isRunning = true;

        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0); // SDL_RENDERER_ACCELERATED is Default
        Uint32 start;
        SDL_Event windowEvent;
        while (isRunning)
        {
            while (SDL_PollEvent(&windowEvent))
            {
                if (windowEvent.type == SDL_QUIT)
                {
                    isRunning = false;
                }

                if (windowEvent.type == SDL_KEYDOWN) {
                    printf("Tapped %d\n", windowEvent.key.keysym.scancode);
                }
            }
            start = SDL_GetTicks();

            // Updates properties of the screen and camera
            // Gets real size of the window(Fix for MacOS/Resizing)
            SDL_GetRendererOutputSize(renderer, &renderWidth, &renderHeight);
            const float fAspectRatio = (float)renderHeight / (float)renderWidth;
            matProj = Matrix4::makeProjection(scene.camera.fFOV, fAspectRatio, scene.camera.fNear, scene.camera.fFar);

            //Background(Clears with color)
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            //Draws scene
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            drawScene();

            // Renders window
            SDL_RenderPresent(renderer);
            Uint32 ticks = SDL_GetTicks();
            if (1000 / targetFps > ticks - start) {
                SDL_Delay(1000 / targetFps - (ticks - start));
            }
        }
        SDL_DestroyRenderer(renderer);
    }

    void drawScene() {
        printf("Drawing %d objects to scene\n", scene.objects.size());
        for (Object &obj : scene.objects) {
            std::vector<float> vertices = obj.getVertices();
            std::vector<float> normals = obj.getNormals();

            // TODO

        }
    }

public:
    const int targetFps = 60;

    Engine() = default;

    bool start(const char* title, int startWidth, int startHeight) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            printf("Error initializing SDL_INIT_VIDEO! SDL_Error: %s\n", SDL_GetError());
            return false;
        }

        renderWidth = startWidth;
        renderHeight = startHeight;

        this->window = SDL_CreateWindow(
                title,
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                renderWidth,
                renderHeight,
                SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE
        );

        if (this->window == nullptr)
        {
            printf("Error creating window! SDL_Error: %s\n", SDL_GetError());
            return false;
        } else {
            printf("Created window(%dx%d)!\n", renderWidth, renderHeight);
            startRenderLoop();
            return true;
        }
    }

    std::string description() {
        std::string s;
        for (const Object &obj : scene.objects) {
            s += obj.name + ", ";
        }
        s += scene.camera.position.toString();
        return s;
    }
};


#endif //V_3D_ENGINE_H
