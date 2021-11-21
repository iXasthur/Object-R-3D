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
    std::vector<Object> objects{};

    Camera camera = Camera({0, 0, 0}, {0, 0, 0});
    Light light = Light(
            {-3.0f, -3.0f, -3.0f},
            {0.1f, {255, 255, 255, 255}},
            {1.0f, {255, 255, 255, 255}}, // Amethyst 155, 89, 182
            {0.4f, {255, 255, 255, 255}}
            );

    Scene() {
        std::string dir = "../objects/";

        std::vector<std::string> names = {"head", "sphere"};
        for (const auto &name : names) {
            Object obj = ObjectLoader::loadObject(dir + name);
            obj.centerPolygonVertices();
            obj.resizeToHeight(2);
            add(obj);
        }

        resetCamera();
    };

    void resetCamera() {
        camera = Camera({0 , 0, 2}, {0, 0, 0});
    }

    void add(const Object &obj) {
        objects.emplace_back(obj);
        reorderObjects(2);
    }

    void reorderObjects(float delimiterSize) {
        Vector3 pos;
        for (int i = 0; i < objects.size(); ++i) {
            if (i > 0) {
                Vector3 lastDim = objects[i - 1].dimension();
                Vector3 dim = objects[i].dimension();
                pos.x += lastDim.x / 2 + dim.x / 2;
                pos.x += delimiterSize;
            }

            objects[i].position.x = pos.x;
        }
    }
};


#endif //V_3D_SCENE_H
