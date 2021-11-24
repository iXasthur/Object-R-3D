//
// Created by iXasthur on 14.11.2020.
//

#ifndef OBJECT_R_3D_SCENE_H
#define OBJECT_R_3D_SCENE_H

#include <vector>
#include <array>
#include <cstdlib>
#include "./object/Object.h"
#include "./light/Light.h"
#include "./camera/Camera.h"
#include "../utils/Color.h"
#include "./object/ObjectLoader.h"

#define TEST_SCENE_0

class Scene {
public:
    std::vector<Object> objects{};

    Camera camera = Camera({0, 0, 0}, {0, 0, 0});
    Light light = Light(
            {-3.0f, -3.0f, -3.0f},
            {0.1f, {255, 255, 255, 255}},
            {1.0f, {255, 255, 255, 255}},
            {0.4f, {255, 255, 255, 255}}
            );

    Scene() {
        std::string dir = "../objects/";

#ifdef TEST_SCENE_0
        std::string name = "cube";
        for (const auto &obj : ObjectLoader::loadObjFile(dir + name)) {
            std::vector<Object> vector = {obj};
            Object::centerVertices(vector);
            Object::resizeToHeight(vector, 1);
            add(vector[0]);
        }
        Object::centerVertices(objects);
        Object::resizeToHeight(objects, 2);
#else
#ifdef TEST_SCENE_1
        std::vector<std::string> names = {"head", "sphere"};
        float h = 2;
        for (const auto &name : names) {
            for (const auto &obj : ObjectLoader::loadObjFile(dir + name)) {
                std::vector<Object> vector = {obj};
                Object::centerVertices(vector);
                Object::resizeToHeight(vector, h++);
                add(vector[0]);
            }
        }
        Object::centerVertices(objects);
        Object::resizeToHeight(objects, 2);
#else
#ifdef TEST_SCENE_2
        std::string name = "molecule";
        for (const auto &obj : ObjectLoader::loadObjFile(dir + name)) {
            add(obj);
        }
        Object::centerVertices(objects);
        Object::resizeToHeight(objects, 2);
#endif
#endif
#endif

        resetCamera();
    };

    void resetCamera() {
        camera = Camera({0 , 0, 4}, {0, 0, 0});
    }

    void add(const Object &obj) {
        objects.emplace_back(obj);
    }

    void reorderObjects(float delimiterSize) {
        Vector3 pos;
        for (int i = 0; i < objects.size(); ++i) {
            if (i > 0) {
                Vector3 lastDim = Object::dimension({objects[i - 1]});
                Vector3 dim = Object::dimension({objects[i]});
                pos.x += lastDim.x / 2 + dim.x / 2;
                pos.x += delimiterSize;
            }

            objects[i].position.x = pos.x;
        }

        for (Object &object : objects) {
            object.position.x -= pos.x / 2;
        }
    }
};


#endif //OBJECT_R_3D_SCENE_H
