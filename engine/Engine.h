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
#include "utils/Triangle2D.h"
#include "renderer/Renderer.h"
#include <cmath>
#include <list>

class Engine {
private:
    SDL_Window *window = nullptr;
    Renderer renderer = Renderer(nullptr);

    int selectedSceneRenderer = 0;

    Scene scene = Scene();

    std::vector<Polygon> clipPolygonByCamera(Polygon &polygon) const {
        return polygon.clipAgainstPlane(
                { 0.0f, 0.0f, scene.camera.fNear },
                { 0.0f, 0.0f, 1.0f }
        );
    }

    std::vector<Polygon> clipPolygonByScreen(Polygon &polygon) {
        std::list<Polygon> listTriangles;

        // Add initial triangle
        listTriangles.push_back(polygon);
        unsigned long nNewTriangles = 1;

        for (int p = 0; p < 4; p++) {
            while (nNewTriangles > 0) {
                std::vector<Polygon> clipped;

                // Take triangle from front of queue
                Polygon test = listTriangles.front();
                listTriangles.pop_front();
                nNewTriangles--;

                // Clip it against a plane. We only need to test each
                // subsequent plane, against subsequent new triangles
                // as all triangles after a plane clip are guaranteed
                // to lie on the inside of the plane. I like how this
                // comment is almost completely and utterly justified
                switch (p) {
                    case 0:
                        clipped = test.clipAgainstPlane(
                                {0.0f, 0.0f, 0.0f},
                                {0.0f, 1.0f, 0.0f});
                        break;
                    case 1:
                        clipped = test.clipAgainstPlane(
                                {0.0f, (float) renderer.getScreenRect().h - 1.0f, 0.0f},
                                {0.0f, -1.0f, 0.0f});
                        break;
                    case 2:
                        clipped = test.clipAgainstPlane(
                                {0.0f, 0.0f, 0.0f},
                                {1.0f, 0.0f, 0.0f});
                        break;
                    case 3:
                        clipped = test.clipAgainstPlane(
                                {(float) renderer.getScreenRect().w - 1.0f, 0.0f, 0.0f},
                                {-1.0f, 0.0f, 0.0f});
                        break;
                }

                // Clipping may yield a variable number of triangles, so
                // add these new ones to the back of the queue for subsequent
                // clipping against next planes
                for (auto &w : clipped) {
                    listTriangles.push_back(w);
                }

            }
            nNewTriangles = listTriangles.size();
        }

        std::vector<Polygon> v;
        v.insert(std::end(v), std::begin(listTriangles), std::end(listTriangles));
        return v;
    }

    std::vector<Polygon> createPolygonsToRaster(Object &obj, Matrix4 &matProj, Matrix4 &matCamView, Matrix4 &matScreen) {
        std::vector<Polygon> polygonsToRaster;

        Matrix4 moveMatrix = Matrix4::makeMove(obj.position.x, obj.position.y, obj.position.z);

        for (Polygon &polygon: obj.polygons) {
            Vector3 normal = polygon.getNormal();
            normal = Vector3::normalize(normal);

            Polygon translated = polygon.matrixMultiplied(moveMatrix);

            Vector3 translatedCenter = translated.vertices[0] + translated.vertices[1] + translated.vertices[2];
            translatedCenter = Vector3::div(translatedCenter, 3);

            Vector3 vCameraRay = Vector3::sub(translatedCenter, scene.camera.position);

            if (Vector3::dotProduct(normal, vCameraRay) < 0.0f) {
                // How similar is normal to light direction
                DirectionalLight light = scene.light;
                light.direction = Vector3::normalize(light.direction);

                float dp = normal.x * -light.direction.x + normal.y * -light.direction.y +
                           normal.z * -light.direction.z;
                if (dp < 0.1f) {
                    dp = 0.1f;
                }

                Color color = polygon.color.exposed(dp);

                // Apply camera transformations
                Polygon viewed = translated.matrixMultiplied(matCamView);

                for (auto &clippedViewed: clipPolygonByCamera(viewed)) {
                    // -1 ... +1
                    Polygon projected = clippedViewed.matrixMultiplied(matProj);

                    // Screen width and height coordinates with inverted Y
                    Polygon screenPolygon = projected.matrixMultiplied(matScreen);

                    for (auto clippedScreenPolygon: clipPolygonByScreen(screenPolygon)) {
                        clippedScreenPolygon.color = color;
                        polygonsToRaster.emplace_back(clippedScreenPolygon);
                    }
                }

            }
        }

        return polygonsToRaster;
    }

    void renderSceneL2() {
        Matrix4 matProj = Matrix4::makeProjection(
                scene.camera.fFOV,
                renderer.getAspectRatio(),
                scene.camera.fNear,
                scene.camera.fFar
        );
        Matrix4 matCameraView = Matrix4::makeCameraView(scene.camera);
        Matrix4 matScreen = Matrix4::makeScreen(renderer.getScreenRect().w, renderer.getScreenRect().h);

        // Store triangles for rastering later
        std::vector<Polygon> polygonsToRaster;
        for (Object &obj: scene.objects) {
            auto polygons = createPolygonsToRaster(obj, matProj, matCameraView, matScreen);
            polygonsToRaster.insert(std::end(polygonsToRaster), std::begin(polygons), std::end(polygons));
        }

        for (Polygon &polygon: polygonsToRaster) {
            renderer.drawFilledScreenPolygon_Z(polygon);
        }

    }

    void renderSceneL1() {
        Matrix4 matProj = Matrix4::makeProjection(
                scene.camera.fFOV,
                renderer.getAspectRatio(),
                scene.camera.fNear,
                scene.camera.fFar
        );
        Matrix4 matCameraView = Matrix4::makeCameraView(scene.camera);
        Matrix4 matScreen = Matrix4::makeScreen(renderer.getScreenRect().w, renderer.getScreenRect().h);

//        printf("Drawing %d objects to scene\n", scene.objects.size());
        for (Object &obj: scene.objects) {
            Matrix4 moveMatrix = Matrix4::makeMove(obj.position.x, obj.position.y, obj.position.z);

            for (Polygon &polygon: obj.polygons) {
                // Move in world
                Polygon translated = polygon.matrixMultiplied(moveMatrix);

                // Apply camera transformations
                Polygon viewed = translated.matrixMultiplied(matCameraView);

                // -1 ... +1
                Polygon projected = viewed.matrixMultiplied(matProj);

                // Screen width and height coordinates with inverted Y
                Polygon screen = projected.matrixMultiplied(matScreen);

                SDL_Point point0 = {(int) screen.vertices[0].x, (int) screen.vertices[0].y};
                SDL_Point point1 = {(int) screen.vertices[1].x, (int) screen.vertices[1].y};
                SDL_Point point2 = {(int) screen.vertices[2].x, (int) screen.vertices[2].y};

                Triangle2D triangleOnScreen = Triangle2D(point0, point1, point2);

                renderer.drawTriangle2D(triangleOnScreen, polygon.color);
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

            // Updates properties of the screen
            // Gets real size of the window (fix for macOS/resizing)
            // + Background (Clears with color)
            renderer.updateScreen({0, 0, 0, 255});

            switch (selectedSceneRenderer) {
                case 0:
                    renderSceneL1();
                    break;
                case 1:
                    renderSceneL2();
                    break;
                default:
                    printf("Invalid scene renderer was selected (%d)\n", selectedSceneRenderer);
                    break;
            }

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
        s += "renderer: " + std::to_string(selectedSceneRenderer);
        return s;
    }
};


#endif //V_3D_ENGINE_H
