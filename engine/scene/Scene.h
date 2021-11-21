//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_SCENE_H
#define V_3D_SCENE_H

#include <vector>
#include <array>
#include <cstdlib>
#include "./object/Object.h"
#include "./light/Light.h"
#include "./camera/Camera.h"
#include "../utils/Color.h"
#include "./object/ObjectLoader.h"

class Scene {
public:
    Object object;
    Camera camera = Camera({0, 0, 0}, {0, 0, 0});
    Light light = Light(
            {-3.0f, -3.0f, -3.0f},
            {0.1f, {255, 255, 255, 255}},
            {1.0f, {255, 255, 255, 255}}, // Amethyst 155, 89, 182
            {0.4f, {255, 255, 255, 255}}
            );

    Scene() {
        std::string dir = "../objects/";

        std::string name = "head";
        Object obj = ObjectLoader::loadObject(dir + name);
        obj.centerPolygonVertices();
        obj.resizeToHeight(2);
        object = obj;

        resetCamera();
    };

    void resetCamera() {
        camera = Camera({0 , 0, 2}, {0, 0, 0});
    }
};


#endif //V_3D_SCENE_H
