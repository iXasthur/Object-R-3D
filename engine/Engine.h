//
// Created by iXasthur on 15.11.2020.
//

#ifndef V_3D_ENGINE_H
#define V_3D_ENGINE_H

#include <array>
#include <SDL2/SDL.h>
#include "scene/Scene.h"
#include "utils/Matrix4.h"
#include "utils/Triangle2D.h"
#include <cmath>

class Engine {
private:
    SDL_Window *window = nullptr;

    Scene scene = Scene();

    int renderWidth = -1;
    int renderHeight = -1;

    Matrix4 matProj = Matrix4();

    void drawTriangle2D(SDL_Renderer *renderer, Triangle2D tr) {
        SDL_Point points[4] = { tr.points[0], tr.points[1], tr.points[2], tr.points[0] };
        // TODO: USE OWN ALGORITHM
        SDL_RenderDrawLines(renderer, points, 4);
    }

    void drawSceneL1(SDL_Renderer *renderer) {
//        // Rotation X, Y, Z
//        Matrix4 matRotX = Matrix4::makeRotationX(0);
//        Matrix4 matRotY = Matrix4::makeRotationY(0);
//        Matrix4 matRotZ = Matrix4::makeRotationZ(0);
//
//        Matrix4 matTrans = Matrix4::makeTranslation(0.0f, 0.0f, 0.0f);
//
//        Matrix4 matWorld = Matrix4::makeIdentity(); // Form World Matrix
//        matWorld = Matrix4::multiplyMatrix(matRotX, matRotY); // Transform by rotation by X and Y
//        matWorld = Matrix4::multiplyMatrix(matWorld, matRotZ); // Transform by rotation by Y
//        matWorld = Matrix4::multiplyMatrix(matWorld, matTrans); // Transform by translation

        Vector3 upVector = { 0, 1, 0 };
        Vector3 targetVector = { 0, 0, 1 };
        Matrix4 m1 = Matrix4::makeRotationX(scene.camera.eulerRotation.x);
        Matrix4 m2 = Matrix4::makeRotationY(scene.camera.eulerRotation.y);
        Matrix4 matCameraRot = Matrix4::multiplyMatrix(m1, m2);
        Vector3 lookDirection = Matrix4::multiplyVector(targetVector, matCameraRot);
        targetVector = Vector3::add(scene.camera.position, lookDirection);
        Matrix4 matCamera = Matrix4::pointAt(scene.camera.position, targetVector, upVector);

        Matrix4 matView = Matrix4::quickInverse(matCamera);

//        printf("Drawing %d objects to scene\n", scene.objects.size());
        for (Object &obj : scene.objects) {
            for (Polygon &polygon : obj.polygons) {
                Vector3 translatedV0 = polygon.vertices[0] + obj.position;
                Vector3 translatedV1 = polygon.vertices[1] + obj.position;
                Vector3 translatedV2 = polygon.vertices[2] + obj.position;

                Vector3 viewedV0 = Matrix4::multiplyVector(translatedV0, matView);
                Vector3 viewedV1 = Matrix4::multiplyVector(translatedV1, matView);
                Vector3 viewedV2 = Matrix4::multiplyVector(translatedV2, matView);

                // -1 ... +1
                Vector3 projectedV0 = Matrix4::multiplyVector(viewedV0, matProj);
                Vector3 projectedV1 = Matrix4::multiplyVector(viewedV1, matProj);
                Vector3 projectedV2 = Matrix4::multiplyVector(viewedV2, matProj);

                // Mirror Y coordinate
                projectedV0.y = -projectedV0.y;
                projectedV1.y = -projectedV1.y;
                projectedV2.y = -projectedV2.y;

                projectedV0.x += 1.0f; projectedV0.y += 1.0f;
                projectedV1.x += 1.0f; projectedV1.y += 1.0f;
                projectedV2.x += 1.0f; projectedV2.y += 1.0f;

                projectedV0.x *= 0.5f * (float)renderWidth; projectedV0.y *= 0.5f * (float)renderHeight;
                projectedV1.x *= 0.5f * (float)renderWidth; projectedV1.y *= 0.5f * (float)renderHeight;
                projectedV2.x *= 0.5f * (float)renderWidth; projectedV2.y *= 0.5f * (float)renderHeight;

                SDL_Point point0 = {(int)projectedV0.x, (int)projectedV0.y};
                SDL_Point point1 = {(int)projectedV1.x, (int)projectedV1.y};
                SDL_Point point2 = {(int)projectedV2.x, (int)projectedV2.y};

                Triangle2D triangleOnScreen = Triangle2D(point0, point1, point2);

                drawTriangle2D(renderer, triangleOnScreen);
            }
        }
    }

    void processInputFast() {
        float positionOffset = 5.0f / (float)targetFps;
        float rotationOffsetRad = 2.0f / (float)targetFps;
        float fovOffset = 20.0f / (float)targetFps;

        const Uint8 *state = SDL_GetKeyboardState(nullptr);

        if (state[SDL_SCANCODE_W]) {
            scene.camera.position.x -= std::sin(scene.camera.eulerRotation.y) * positionOffset;
            scene.camera.position.z += std::cos(scene.camera.eulerRotation.y) * positionOffset;
        }

        if (state[SDL_SCANCODE_S]) {
            scene.camera.position.x += std::sin(scene.camera.eulerRotation.y) * positionOffset;
            scene.camera.position.z -= std::cos(scene.camera.eulerRotation.y) * positionOffset;
        }

        if (state[SDL_SCANCODE_A]) {
            scene.camera.position.z -= std::sin(scene.camera.eulerRotation.y) * positionOffset;
            scene.camera.position.x -= std::cos(scene.camera.eulerRotation.y) * positionOffset;
        }

        if (state[SDL_SCANCODE_D]) {
            scene.camera.position.z += std::sin(scene.camera.eulerRotation.y) * positionOffset;
            scene.camera.position.x += std::cos(scene.camera.eulerRotation.y) * positionOffset;
        }

        if (state[SDL_SCANCODE_SPACE]) {
            scene.camera.position.y += positionOffset;
        }

        if (state[SDL_SCANCODE_X]) {
            scene.camera.position.y -= positionOffset;
        }

        if (state[SDL_SCANCODE_UP]) {
            if (scene.camera.eulerRotation.x - rotationOffsetRad > -M_PI / 2.0f) {
                scene.camera.eulerRotation.x -= rotationOffsetRad;
            } else {
                scene.camera.eulerRotation.x = -M_PI / 2.0f + 0.01f;
            }
        }

        if (state[SDL_SCANCODE_DOWN]) {
            if (scene.camera.eulerRotation.x + rotationOffsetRad < M_PI / 2.0f) {
                scene.camera.eulerRotation.x += rotationOffsetRad;
            } else {
                scene.camera.eulerRotation.x = M_PI / 2.0f - 0.01f;
            }
        }

        if (state[SDL_SCANCODE_LEFT]) {
            scene.camera.eulerRotation.y += rotationOffsetRad;
        }

        if (state[SDL_SCANCODE_RIGHT]) {
            scene.camera.eulerRotation.y -= rotationOffsetRad;
        }

        if (state[SDL_SCANCODE_RIGHTBRACKET]) {
            if (scene.camera.fFOV > 0 + 1) {
                scene.camera.fFOV -= fovOffset;
            }
        }

        if (state[SDL_SCANCODE_LEFTBRACKET]) {
            if (scene.camera.fFOV < 180 - 1) {
                scene.camera.fFOV += fovOffset;
            }
        }

        printf("Camera position: x:%f y:%f z:%f, ", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z);
        printf("euler angle: x:%f y:%f z:%f, ", scene.camera.eulerRotation.x, scene.camera.eulerRotation.y, scene.camera.eulerRotation.z);
        printf("fov: %f\n", scene.camera.fFOV);
    }

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
                    if (windowEvent.key.keysym.scancode == SDL_SCANCODE_R) {
                        scene.resetCamera();
                    }
                }
            }
            start = SDL_GetTicks();

            processInputFast();

            // Updates properties of the screen and camera
            // Gets real size of the window(Fix for macOS/Resizing)
            SDL_GetRendererOutputSize(renderer, &renderWidth, &renderHeight);
            const float fAspectRatio = (float)renderHeight / (float)renderWidth;
            matProj = Matrix4::makeProjection(scene.camera.fFOV, fAspectRatio, scene.camera.fNear, scene.camera.fFar);

            //Background(Clears with color)
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            //Draws scene
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            drawSceneL1(renderer);

            updateWindowTitle();

            // Renders window
            SDL_RenderPresent(renderer);
            Uint32 ticks = SDL_GetTicks();
            if (1000 / targetFps > ticks - start) {
                SDL_Delay(1000 / targetFps - (ticks - start));
            }
        }
        SDL_DestroyRenderer(renderer);
    }

    void updateWindowTitle() {
        SDL_SetWindowTitle(window, description().c_str());
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
