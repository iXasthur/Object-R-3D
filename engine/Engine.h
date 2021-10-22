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

    [[nodiscard]] std::vector<Polygon> clipPolygonByCamera(const Polygon &polygon) const {
        Vector3 target = scene.camera.getInitialTargetVector();

        Plane nearPlane = Plane({0.0f, 0.0f, scene.camera.fNear}, {target.x, target.y, target.z});
        Plane farPlane = Plane({0.0f, 0.0f, scene.camera.fFar}, {target.x, target.y, -target.z});

        std::vector<Polygon> clippedNear = polygon.clipAgainstPlane(nearPlane);

        std::vector<Polygon> clippedFar;
        for (const auto &p: clippedNear) {
            std::vector<Polygon> clipped = polygon.clipAgainstPlane(farPlane);
            clippedFar.insert(std::end(clippedFar), std::begin(clipped), std::end(clipped));
        }

        return clippedFar;
    }
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

    void renderObject(const Object &obj) {
        Matrix4 matMove = Matrix4::makeMove(obj.position.x, obj.position.y, obj.position.z);

        for (const Polygon &polygon: obj.polygons) {
            // Force flat shading
//                for (auto &item : polygon.vertices) {
//                    item.normal = polygon.getFaceNormal();
//                }

            Vector3 normal = Vector3::normalize(polygon.getFaceNormal());

            Polygon translated = polygon.matrixMultiplied(matMove);

            Vector3 translatedCenter = translated.getCenter();

            Vector3 vCameraRay = Vector3::sub(translatedCenter, scene.camera.position);

            if (Vector3::dotProduct(normal, vCameraRay) < 0.0f) {
                // Apply camera transformations
                Polygon viewed = translated.matrixMultiplied(renderer.matCameraView);

                for (const auto &clippedViewed: clipPolygonByCamera(viewed)) {
                    // -1 ... +1
                    Polygon projected = clippedViewed.matrixMultiplied(renderer.matProj);

                    // Screen width and height coordinates with inverted Y
                    Polygon screenPolygon = projected.matrixMultiplied(renderer.matScreen);

                    renderer.drawPolygon(screenPolygon, scene.camera, scene.light, obj.color, obj.shininess);
                }

//                    std::cout << "----" << std::endl;
//
//                    std::cout << "   m: " << polygon.vertices[0].position.toString() << std::endl;
//                    std::cout << "   w: " << translated.vertices[0].position.toString() << std::endl;
//                    std::cout << "   v: " << viewed.vertices[0].position.toString() << std::endl;
//                    std::cout << "   p: " << projected.vertices[0].position.toString() << std::endl;
//                    std::cout << "   s: " << screenPolygon.vertices[0].position.toString() << std::endl;
//
//                    Polygon i_screen = screenPolygon.matrixMultiplied(Matrix4::invert(renderer.matScreen));
//                    std::cout << "s->p: " << i_screen.vertices[0].position.toString() << std::endl;
//
//                    Polygon i_projected = i_screen.matrixMultiplied(Matrix4::invert(renderer.matProj));
//                    std::cout << "p->v: " << i_projected.vertices[0].position.toString() << std::endl;
//
//                    Polygon i_viewed = i_projected.matrixMultiplied(Matrix4::invert(renderer.matCameraView));
//                    std::cout << "v->w: " << i_viewed.vertices[0].position.toString() << std::endl;
//
//                    Polygon i_world = i_viewed.matrixMultiplied(Matrix4::invert(renderer.matMove));
//                    std::cout << "w->m: " << i_world.vertices[0].position.toString() << std::endl;
            }
        }
    }

    void renderScene() {
//        scene.light.position = Matrix4::multiplyVector(scene.light.position, Matrix4::makeRotationY(0.02f));

        for (const Object &obj: scene.objects) {
            renderObject(obj);
        }

        if (scene.light.model != nullptr) {
            Object lightObject = scene.light.getObject();
            renderObject(lightObject);
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

            // Updates properties of the screen
            // Gets real size of the window (fix for macOS/resizing)
            // + Background (Clears with color)
            renderer.updateScreen({20, 20, 20, 255});

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

            renderer.updateMatrices(matCameraView, matProj, matScreen);

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

        if (this->window == nullptr) {
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
        s += "light_position: " + scene.light.position.toString();
        return s;
    }
};


#endif //V_3D_ENGINE_H
