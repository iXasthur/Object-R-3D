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

    int selectedSceneRenderer = 1;

    Scene scene = Scene();

    std::vector<Polygon> clipPolygonByCamera(Polygon &polygon) const {
        Polygon clipped[2];
        int nClippedTriangles = Polygon::clipAgainstPlane(
                { 0.0f, 0.0f, scene.camera.fNear },
                { 0.0f, 0.0f, 1.0f },
                polygon,
                clipped[0],
                clipped[1]
        );

        std::vector<Polygon> v;
        for (int n = 0; n < nClippedTriangles; n++) {
            v.emplace_back(clipped[n]);
        }

        return v;
    }

    std::vector<Polygon> clipPolygonByScreen(Polygon &polygon) {
        Polygon clipped[2];
        std::list<Polygon> listTriangles;

        // Add initial triangle
        listTriangles.push_back(polygon);
        unsigned long nNewTriangles = 1;

        for (int p = 0; p < 4; p++) {
            int nTrisToAdd = 0;
            while (nNewTriangles > 0) {
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
                        nTrisToAdd = Polygon::clipAgainstPlane({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, test,
                                                               clipped[0], clipped[1]);
                        break;
                    case 1:
                        nTrisToAdd = Polygon::clipAgainstPlane({0.0f, (float) renderer.getScreenRect().h - 1.0f, 0.0f},
                                                               {0.0f, -1.0f, 0.0f}, test, clipped[0], clipped[1]);
                        break;
                    case 2:
                        nTrisToAdd = Polygon::clipAgainstPlane({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, test,
                                                               clipped[0], clipped[1]);
                        break;
                    case 3:
                        nTrisToAdd = Polygon::clipAgainstPlane({(float) renderer.getScreenRect().w - 1.0f, 0.0f, 0.0f},
                                                               {-1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
                        break;
                }

                // Clipping may yield a variable number of triangles, so
                // add these new ones to the back of the queue for subsequent
                // clipping against next planes
                for (int w = 0; w < nTrisToAdd; w++) {
                    listTriangles.push_back(clipped[w]);
                }

            }
            nNewTriangles = listTriangles.size();
        }

        std::vector<Polygon> v;
        v.insert(std::end(v), std::begin(listTriangles), std::end(listTriangles));
        return v;
    }

    std::vector<Polygon> createRasterizedPolygons(Object &obj, Matrix4 &matProj, Matrix4 &matCamView, Matrix4 &matScreen) {
        std::vector<Polygon> rasterizedPolygons;

        Matrix4 moveMatrix = Matrix4::makeMove(obj.position.x, obj.position.y, obj.position.z);

        for (Polygon &polygon: obj.polygons) {
            Vector3 normal = polygon.getNormal();
            normal = Vector3::normalize(normal);

            Vector3 translatedV0 = Matrix4::multiplyVector(polygon.vertices[0], moveMatrix);
            Vector3 translatedV1 = Matrix4::multiplyVector(polygon.vertices[1], moveMatrix);
            Vector3 translatedV2 = Matrix4::multiplyVector(polygon.vertices[2], moveMatrix);

            Vector3 translatedCenter = translatedV0 + translatedV1 + translatedV2;
            translatedCenter = Vector3::div(translatedCenter, 3);

            Vector3 vCameraRay = Vector3::sub(translatedCenter, scene.camera.position);

            if (Vector3::dotProduct(normal, vCameraRay) < 0.0f) {
                // How similar is normal to light direction
                float dp = normal.x * -scene.light.direction.x + normal.y * -scene.light.direction.y +
                           normal.z * -scene.light.direction.z;
                if (dp < 0.1f) {
                    dp = 0.1f;
                }

                Color color = polygon.color.exposed(dp);

                // Apply camera transformations
                Vector3 viewedV0 = Matrix4::multiplyVector(translatedV0, matCamView);
                Vector3 viewedV1 = Matrix4::multiplyVector(translatedV1, matCamView);
                Vector3 viewedV2 = Matrix4::multiplyVector(translatedV2, matCamView);

                Polygon viewedPolygon = {viewedV0, viewedV1, viewedV2};
                for (auto &clippedViewedPolygon: clipPolygonByCamera(viewedPolygon)) {
                    // -1 ... +1
                    Vector3 projectedV0 = Matrix4::multiplyVector(clippedViewedPolygon.vertices[0], matProj);
                    Vector3 projectedV1 = Matrix4::multiplyVector(clippedViewedPolygon.vertices[1], matProj);
                    Vector3 projectedV2 = Matrix4::multiplyVector(clippedViewedPolygon.vertices[2], matProj);

                    // Screen width and height coordinates with inverted Y
                    Vector3 screenV0 = Matrix4::multiplyVector(projectedV0, matScreen);
                    Vector3 screenV1 = Matrix4::multiplyVector(projectedV1, matScreen);
                    Vector3 screenV2 = Matrix4::multiplyVector(projectedV2, matScreen);

                    Polygon screenPolygon = {screenV0, screenV1, screenV2};
                    for (auto clippedScreenPolygon: clipPolygonByScreen(screenPolygon)) {
                        clippedScreenPolygon.color = color;
                        rasterizedPolygons.emplace_back(clippedScreenPolygon);
                    }
                }

            }
        }

        return rasterizedPolygons;
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

        // Store triagles for rastering later
        std::vector<Polygon> rasterizedPolygons;
        for (Object &obj: scene.objects) {
            auto rasterized = createRasterizedPolygons(obj, matProj, matCameraView, matScreen);
            rasterizedPolygons.insert(std::end(rasterizedPolygons), std::begin(rasterized), std::end(rasterized));
        }

        sort(rasterizedPolygons.begin(), rasterizedPolygons.end(), [](Polygon &p0, Polygon &p1) {
            float z0 = (p0.vertices[0].z + p0.vertices[1].z + p0.vertices[2].z) / 3.0f;
            float z1 = (p1.vertices[0].z + p1.vertices[1].z + p1.vertices[2].z) / 3.0f;
            return z0 > z1;
        });

        for (Polygon &polygon: rasterizedPolygons) {
            SDL_Point point0 = {(int) polygon.vertices[0].x, (int) polygon.vertices[0].y};
            SDL_Point point1 = {(int) polygon.vertices[1].x, (int) polygon.vertices[1].y};
            SDL_Point point2 = {(int) polygon.vertices[2].x, (int) polygon.vertices[2].y};

            Triangle2D triangleOnScreen = Triangle2D(point0, point1, point2);

            renderer.drawFilledTriangle2D(triangleOnScreen, polygon.color);
//            renderer.drawTriangle2D(triangleOnScreen, polygon.color);
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
            renderer.updateScreenRect();

            //Background (Clears with color)
            renderer.clear({0, 0, 0, 255});

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
        s += scene.camera.position.toString();
        return s;
    }
};


#endif //V_3D_ENGINE_H
