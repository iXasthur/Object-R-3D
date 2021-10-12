//
// Created by iXasthur on 15.11.2020.
//

#ifndef V_3D_ENGINE_H
#define V_3D_ENGINE_H

#include <array>
#include <algorithm>
#include <SDL2/SDL.h>
#include "scene/Scene.h"
#include "utils/Matrix4.h"
#include "renderer/Renderer.h"
#include <cmath>
#include <list>
#include <iomanip>

class Engine {
private:
    SDL_Window *window = nullptr;
    Renderer renderer = Renderer(nullptr);

    Scene scene = Scene();

//    [[nodiscard]] std::vector<Polygon> clipPolygonByCamera(const Polygon &polygon) const {
//        Vector3 target = scene.camera.getInitialTargetVector();
//
//        std::vector<Polygon> clippedNear = polygon.clipAgainstPlane(
//                { 0.0f, 0.0f, scene.camera.fNear },
//                {target.x, target.y, target.z}
//        );
//
//        std::vector<Polygon> clippedFar;
//
//        for (const auto &p : clippedNear) {
//            std::vector<Polygon> clipped = polygon.clipAgainstPlane(
//                    { 0.0f, 0.0f, scene.camera.fFar },
//                    {target.x, target.y, -target.z}
//            );
//
//            clippedFar.insert(std::end(clippedFar), std::begin(clipped), std::end(clipped));
//        }
//
//        return clippedFar;
//    }
//
//    [[nodiscard]] std::vector<Polygon> clipPolygonByScreen(const Polygon &polygon) const {
//        std::list<Polygon> listTriangles;
//
//        // Add initial triangle
//        listTriangles.push_back(polygon);
//        unsigned long nNewTriangles = 1;
//
//        for (int p = 0; p < 4; p++) {
//            while (nNewTriangles > 0) {
//                std::vector<Polygon> clipped;
//
//                // Take triangle from front of queue
//                Polygon test = listTriangles.front();
//                listTriangles.pop_front();
//                nNewTriangles--;
//
//                // Clip it against a plane. We only need to test each
//                // subsequent plane, against subsequent new triangles
//                // as all triangles after a plane clip are guaranteed
//                // to lie on the inside of the plane. I like how this
//                // comment is almost completely and utterly justified
//                switch (p) {
//                    case 0:
//                        clipped = test.clipAgainstPlane(
//                                {0.0f, 0.0f, 0.0f},
//                                {0.0f, 1.0f, 0.0f});
//                        break;
//                    case 1:
//                        clipped = test.clipAgainstPlane(
//                                {0.0f, (float) renderer.getScreenRect().h - 1.0f, 0.0f},
//                                {0.0f, -1.0f, 0.0f});
//                        break;
//                    case 2:
//                        clipped = test.clipAgainstPlane(
//                                {0.0f, 0.0f, 0.0f},
//                                {1.0f, 0.0f, 0.0f});
//                        break;
//                    case 3:
//                        clipped = test.clipAgainstPlane(
//                                {(float) renderer.getScreenRect().w - 1.0f, 0.0f, 0.0f},
//                                {-1.0f, 0.0f, 0.0f});
//                        break;
//                    default:
//                        break;
//                }
//
//                // Clipping may yield a variable number of triangles, so
//                // add these new ones to the back of the queue for subsequent
//                // clipping against next planes
//                for (auto &w : clipped) {
//                    listTriangles.push_back(w);
//                }
//
//            }
//            nNewTriangles = listTriangles.size();
//        }
//
//        std::vector<Polygon> v;
//        v.insert(std::end(v), std::begin(listTriangles), std::end(listTriangles));
//        return v;
//    }

    void renderScene() {
        Matrix4 matProj = Matrix4::makeProjection(
                scene.camera.fFOV,
                renderer.getAspectRatio(),
                scene.camera.fNear,
                scene.camera.fFar
        );
        Matrix4 matCameraView = Matrix4::makeCameraView(scene.camera);
        Matrix4 matScreen = Matrix4::makeScreen(renderer.getScreenRect().w, renderer.getScreenRect().h);

        // TODO: Draw light object

        for (const Object &obj: scene.objects) {
            Matrix4 moveMatrix = Matrix4::makeMove(obj.position.x, obj.position.y, obj.position.z);

            for (const Polygon &polygon: obj.polygons) {
                Vector3 normal = Vector3::normalize(polygon.getFaceNormal());

                Polygon translated = polygon.matrixMultiplied(moveMatrix);

                Vector3 translatedCenter = Vector3::add(Vector3::add(translated.vertices[0], translated.vertices[1]), translated.vertices[2]);
                translatedCenter = Vector3::div(translatedCenter, 3);

                Vector3 vCameraRay = Vector3::sub(translatedCenter, scene.camera.position);

                if (Vector3::dotProduct(normal, vCameraRay) < 0.0f) {
                    // Apply camera transformations
                    Polygon viewed = translated.matrixMultiplied(matCameraView);

                    // -1 ... +1
                    Polygon projected = viewed.matrixMultiplied(matProj);

                    // Screen width and height coordinates with inverted Y
                    Polygon screenPolygon = projected.matrixMultiplied(matScreen);

                    renderer.drawPolygon(screenPolygon, scene.light, obj.color);

//                    for (const auto &clippedViewed: clipPolygonByCamera(viewed)) {
//                        // -1 ... +1
//                        Polygon projected = clippedViewed.matrixMultiplied(matProj);
//
//                        // Screen width and height coordinates with inverted Y
//                        Polygon screenPolygon = projected.matrixMultiplied(matScreen);
//
//                        renderer.drawPolygon(screenPolygon, light, obj.color);
//                    }

                }
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

        SDL_Renderer *sdl_renderer = SDL_CreateRenderer(window, -1, 0); // SDL_RENDERER_ACCELERATED is Default
        renderer = Renderer(sdl_renderer);

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
                    }
                }
            }
            start = SDL_GetTicks();

            processInputFast();

            // Updates properties of the screen
            // Gets real size of the window (fix for macOS/resizing)
            // + Background (Clears with color)
            renderer.updateScreen({20, 20, 20, 255});

            renderScene();

            updateWindowTitle();

            // Renders window
            SDL_RenderPresent(sdl_renderer);
            Uint32 ticks = SDL_GetTicks();
            if (1000 / targetFps > ticks - start) {
                SDL_Delay(1000 / targetFps - (ticks - start));
            }
        }

        SDL_DestroyRenderer(sdl_renderer);
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

        this->window = SDL_CreateWindow(
                title,
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                startWidth,
                startHeight,
                SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE
        );

        if (this->window == nullptr)
        {
            printf("Error creating window! SDL_Error: %s\n", SDL_GetError());
            return false;
        } else {
            printf("Created window(%dx%d)!\n", startWidth, startHeight);
            startRenderLoop();
            return true;
        }
    }

    std::string description() {
        std::string s;
        for (const Object &obj : scene.objects) {
            s += obj.name + ", ";
        }
        s += scene.camera.position.toString() + ", ";
        s += "light_direction: " + scene.light.direction.toString();
        return s;
    }
};


#endif //V_3D_ENGINE_H
