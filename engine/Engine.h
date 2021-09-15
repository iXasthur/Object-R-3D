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

    int selectedSceneRenderer = 0;

    Scene scene = Scene();

    SDL_Rect screenRect = SDL_Rect();

    void drawLine(SDL_Renderer *renderer, SDL_Point p0, SDL_Point p1) const {
        if (!SDL_IntersectRectAndLine(&screenRect, &p0.x, &p0.y, &p1.x, &p1.y)) {
            return;
        }

        int x0 = p0.x;
        int y0 = p0.y;
        int x1 = p1.x;
        int y1 = p1.y;

        int dx = abs(x1 - x0);
        int sx = x0 < x1 ? 1 : -1;

        int dy = -abs(y1 - y0);
        int sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;  /* error value e_xy */
        while (true) {
            SDL_RenderDrawPoint(renderer, x0, y0);
            if (x0 == x1 && y0 == y1) {
                break;
            }
            int e2 = 2 * err;
            if (e2 >= dy) { /* e_xy+e_x > 0 */
                err += dy;
                x0 += sx;
            }
            if (e2 <= dx) { /* e_xy+e_y < 0 */
                err += dx;
                y0 += sy;
            }
        }
    }

    void drawTriangle2D(SDL_Renderer *renderer, Triangle2D tr) {
        drawLine(renderer, tr.points[0], tr.points[1]);
        drawLine(renderer, tr.points[1], tr.points[2]);
        drawLine(renderer, tr.points[2], tr.points[0]);
//        SDL_Point points[4] = { tr.points[0], tr.points[1], tr.points[2], tr.points[0] };
//        SDL_RenderDrawLines(renderer, points, 4);
    }

    void renderSceneL2(SDL_Renderer *renderer) {

    }

    void renderSceneL1(SDL_Renderer *renderer) {
        const float fAspectRatio = (float) screenRect.h / (float) screenRect.w;
        Matrix4 matProj = Matrix4::makeProjection(scene.camera.fFOV, fAspectRatio, scene.camera.fNear, scene.camera.fFar);

        Matrix4 matCameraView = Matrix4::makeCameraView(scene.camera);
        Matrix4 matScreen = Matrix4::makeScreen(screenRect.w, screenRect.h);

//        printf("Drawing %d objects to scene\n", scene.objects.size());
        for (Object &obj: scene.objects) {
            Matrix4 moveMatrix = Matrix4::makeMove(obj.position.x, obj.position.y, obj.position.z);

            for (Polygon &polygon: obj.polygons) {
                // Move in world
                Vector3 translatedV0 = Matrix4::multiplyVector(polygon.vertices[0], moveMatrix);
                Vector3 translatedV1 = Matrix4::multiplyVector(polygon.vertices[1], moveMatrix);
                Vector3 translatedV2 = Matrix4::multiplyVector(polygon.vertices[2], moveMatrix);

                // Apply camera transformations
                Vector3 viewedV0 = Matrix4::multiplyVector(translatedV0, matCameraView);
                Vector3 viewedV1 = Matrix4::multiplyVector(translatedV1, matCameraView);
                Vector3 viewedV2 = Matrix4::multiplyVector(translatedV2, matCameraView);

                // -1 ... +1
                Vector3 projectedV0 = Matrix4::multiplyVector(viewedV0, matProj);
                Vector3 projectedV1 = Matrix4::multiplyVector(viewedV1, matProj);
                Vector3 projectedV2 = Matrix4::multiplyVector(viewedV2, matProj);

                // Screen width and height coordinates with inverted Y
                Vector3 screenV0 = Matrix4::multiplyVector(projectedV0, matScreen);
                Vector3 screenV1 = Matrix4::multiplyVector(projectedV1, matScreen);
                Vector3 screenV2 = Matrix4::multiplyVector(projectedV2, matScreen);

                SDL_Point point0 = {(int) screenV0.x, (int) screenV0.y};
                SDL_Point point1 = {(int) screenV1.x, (int) screenV1.y};
                SDL_Point point2 = {(int) screenV2.x, (int) screenV2.y};

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

//        printf("Camera position: x:%f y:%f z:%f, ", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z);
//        printf("euler angle: x:%f y:%f z:%f, ", scene.camera.eulerRotation.x, scene.camera.eulerRotation.y, scene.camera.eulerRotation.z);
//        printf("fov: %f\n", scene.camera.fFOV);
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
                    switch (windowEvent.key.keysym.scancode) {
                        case SDL_SCANCODE_R:
                            scene.resetCamera();
                            break;
                        case SDL_SCANCODE_1:
                            selectedSceneRenderer = 0;
                            break;
                        case SDL_SCANCODE_2:
                            selectedSceneRenderer = 1;
                            break;
                    }
                }
            }
            start = SDL_GetTicks();

            processInputFast();

            // Updates properties of the screen and camera
            // Gets real size of the window(Fix for macOS/Resizing)
            SDL_GetRendererOutputSize(renderer, &screenRect.w, &screenRect.h);

            //Background(Clears with color)
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            //Draws scene
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            switch (selectedSceneRenderer) {
                case 0:
                    renderSceneL1(renderer);
                    break;
                case 1:
                    renderSceneL2(renderer);
                    break;
                default:
                    printf("Invalid scene renderer was selected (%d)\n", selectedSceneRenderer);
                    break;
            }

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

        screenRect.w = startWidth;
        screenRect.h = startHeight;

        this->window = SDL_CreateWindow(
                title,
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                screenRect.w,
                screenRect.h,
                SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE
        );

        if (this->window == nullptr)
        {
            printf("Error creating window! SDL_Error: %s\n", SDL_GetError());
            return false;
        } else {
            printf("Created window(%dx%d)!\n", screenRect.w, screenRect.h);
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
