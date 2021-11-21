//
// Created by iXasthur on 15.11.2020.
//

#ifndef V_3D_ENGINE_H
#define V_3D_ENGINE_H

#include <array>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>
#include "scene/Scene.h"
#include "utils/Matrix4.h"
#include "renderer/Renderer.h"
#include <cmath>
#include <list>
#include <iomanip>
#include <thread>
#include <future>

class Engine {
private:
    SDL_Window *window = nullptr;
    Renderer renderer = Renderer(nullptr);

    Scene scene = Scene();

    Uint32 frameDeltaTime = 0;

    [[nodiscard]] std::vector<Polygon> clipPolygonByCamera(const Polygon &polygon) const {
        Plane nearPlane = Plane({0.0f, 0.0f, scene.camera.fNear}, {0.0f, 0.0f, 1.0f});
        Plane farPlane = Plane({0.0f, 0.0f, scene.camera.fFar}, {0.0f, 0.0f, -1.0f});

        std::vector<Polygon> clippedNear = polygon.clipAgainstPlane(nearPlane);

        std::vector<Polygon> clippedFar;
        for (const auto &p: clippedNear) {
            std::vector<Polygon> clipped = polygon.clipAgainstPlane(farPlane);
            clippedFar.insert(std::end(clippedFar), std::begin(clipped), std::end(clipped));
        }

        return clippedFar;
    }

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

    void renderObject(const Object &obj) {
        std::vector<Pixel> pixels;

        Matrix4 matMove = Matrix4::makeMove(obj.position.x, obj.position.y, obj.position.z);

        auto task = [&obj, &matMove, this](int start_polygon, int end_polygon, std::promise<std::vector<Pixel>> && ret) {
            std::vector<Pixel> pixels;

            for (int i = start_polygon; i <= end_polygon; i++) {
                const Polygon &polygon = obj.polygons[i];

                Vector3 normal = Vector3::normalize(polygon.getFaceNormal());
                Polygon translated = polygon.matrixMultiplied(matMove);
                Vector3 translatedCenter = translated.getCenter();
                Vector3 vCameraRay = Vector3::sub(translatedCenter, scene.camera.position);

                if (Vector3::dotProduct(normal, vCameraRay) < 0.0f) {
                    Polygon viewed = translated.matrixMultiplied(renderer.matCameraView);

                    for (const auto &clippedViewed: clipPolygonByCamera(viewed)) {
                        Polygon projected = clippedViewed.matrixMultiplied(renderer.matProj);
                        Polygon screenPolygon = projected.matrixMultiplied(renderer.matScreen);

                        auto ps = renderer.processPolygon(screenPolygon, scene);
                        pixels.insert(pixels.end(), ps.begin(), ps.end());
                    }

                }
            }

            ret.set_value(pixels);
        };

        int threads_count = (int) std::thread::hardware_concurrency();
        int polygons_count = (int) obj.polygons.size();
        int thread_polygons_count = std::ceil((float) polygons_count / (float) threads_count);

        std::vector<std::future<std::vector<Pixel>>> futures;

        int start = 0;
        int end = thread_polygons_count;
        while (start <= polygons_count - 1) {
            std::promise<std::vector<Pixel>> thread_promise;
            futures.emplace_back(thread_promise.get_future());
            auto thread = std::thread(task, start, end, std::move(thread_promise));
            thread.detach();

            start += thread_polygons_count;
            end += thread_polygons_count;

            if (end > polygons_count - 1) {
                end = polygons_count - 1;
            }
        }

        int i = 0;
        while (!futures.empty()) {
            if (i > futures.size() - 1) {
                i = 0;
            }

            if (futures[i].wait_for(std::literals::chrono_literals::operator""ns(0ull)) == std::future_status::ready) {
                renderer.drawPixels(futures[i].get());
                futures.erase(futures.begin() + i);
            }

            i++;
        }
    }

    void renderScene() {
        renderObject(scene.object);
    }

    void processInputFast() {
//        float positionOffset = 5.0f / (float) targetFps;
//        float rotationOffsetRad = 2.0f / (float) targetFps;
//        float fovOffset = 20.0f / (float) targetFps;

        auto dt = (float) frameDeltaTime / 1000.0f;
        float positionOffset = 2.0f * dt;
        float rotationOffsetRad = 1.0f * dt;
        float fovOffset = 30.0f * dt;

        const Uint8 *state = SDL_GetKeyboardState(nullptr);

        if (state[SDL_SCANCODE_W]) {
            scene.camera.position.x -= std::sin(scene.camera.eulerRotation.y) * positionOffset;
            scene.camera.position.z -= std::cos(scene.camera.eulerRotation.y) * positionOffset;
        }

        if (state[SDL_SCANCODE_S]) {
            scene.camera.position.x += std::sin(scene.camera.eulerRotation.y) * positionOffset;
            scene.camera.position.z += std::cos(scene.camera.eulerRotation.y) * positionOffset;
        }

        if (state[SDL_SCANCODE_A]) {
            scene.camera.position.x -= std::cos(scene.camera.eulerRotation.y) * positionOffset;
            scene.camera.position.z += std::sin(scene.camera.eulerRotation.y) * positionOffset;
        }

        if (state[SDL_SCANCODE_D]) {
            scene.camera.position.x += std::cos(scene.camera.eulerRotation.y) * positionOffset;
            scene.camera.position.z -= std::sin(scene.camera.eulerRotation.y) * positionOffset;
        }

        if (state[SDL_SCANCODE_SPACE]) {
            scene.camera.position.y += positionOffset;
        }

        if (state[SDL_SCANCODE_X]) {
            scene.camera.position.y -= positionOffset;
        }

        if (state[SDL_SCANCODE_UP]) {
            if (scene.camera.eulerRotation.x + rotationOffsetRad < M_PI / 2.0f) {
                scene.camera.eulerRotation.x += rotationOffsetRad;
            } else {
                scene.camera.eulerRotation.x = M_PI / 2.0f - 0.01f;
            }
        }

        if (state[SDL_SCANCODE_DOWN]) {
            if (scene.camera.eulerRotation.x - rotationOffsetRad > -M_PI / 2.0f) {
                scene.camera.eulerRotation.x -= rotationOffsetRad;
            } else {
                scene.camera.eulerRotation.x = -M_PI / 2.0f + 0.01f;
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

        frameDeltaTime = 1000 / targetFps;

        Uint32 start;
        SDL_Event windowEvent;
        while (isRunning) {
            while (SDL_PollEvent(&windowEvent)) {
                if (windowEvent.type == SDL_QUIT) {
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

            Matrix4 matCameraView = Matrix4::makeCameraView(
                    scene.camera.getInitialUpVector(),
                    scene.camera.getInitialTargetVector(),
                    scene.camera.position,
                    scene.camera.eulerRotation
            );
            Matrix4 matProj = Matrix4::makeProjection(
                    scene.camera.fFOV,
                    renderer.getAspectRatio(),
                    scene.camera.fNear,
                    scene.camera.fFar
            );
            Matrix4 matScreen = Matrix4::makeScreen(renderer.getScreenRect().w, renderer.getScreenRect().h);

            // Updates properties of the screen
            // Gets real size of the window (fix for macOS/resizing)
            // + Background (Clears with color)
            renderer.updateScreen({20, 20, 20, 255}, matCameraView, matProj, matScreen);

            renderScene();

            updateWindowTitle();

            // Renders window
            SDL_RenderPresent(sdl_renderer);
            frameDeltaTime = SDL_GetTicks() - start;
            if (1000 / targetFps > frameDeltaTime) {
                SDL_Delay(1000 / targetFps - frameDeltaTime);
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

        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags) & imgFlags))
        {
            printf("Error initializing IMG_INIT_PNG! IMG_Error: %s\n", IMG_GetError());
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

        if (this->window == nullptr) {
            printf("Error creating window! SDL_Error: %s\n", SDL_GetError());
            return false;
        } else {
            printf("Created window(%dx%d)!\n", startWidth, startHeight);
            startRenderLoop();
            return true;
        }
    }

    [[nodiscard]] std::string description() const {
        std::string fov = std::to_string(scene.camera.fFOV);
        fov = fov.substr(0, fov.find('.') + 3);

        std::string s = scene.object.name + "; ";
        s += "camera: " + scene.camera.position.toString() + ", ";
        s += "fov: " + fov + ", ";
        s += "euler: " + scene.camera.eulerRotation.toString() + ", ";
        s += "light: " + scene.light.direction.toString();
        return s;
    }
};


#endif //V_3D_ENGINE_H
