//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_SCENE_H
#define V_3D_SCENE_H

#include <vector>
#include <array>
#include <cstdlib>
#include "../object/Object.h"
#include "../light/Light.h"
#include "../camera/Camera.h"
#include "../utils/Color.h"

class Scene {
public:
    std::vector<Object> objects = {Object::createCube()};
    Camera camera = Camera({0 , 0, -10});

    Scene() = default;

    void resetCamera() {
        camera.position = {0 , 0, -10};
        camera.eulerRotation = EulerAngle();
    }

    void add(const Object &obj) {
        objects.emplace_back(obj);
        reorderObjects();
    }

    void reorderObjects() {
        Vector3 pos;
        for (int i = 0; i < objects.size(); ++i) {
            if (i > 0) {
                Vector3 lastDim = objects[i - 1].dimension();
                Vector3 dim = objects[i].dimension();
                pos.x += lastDim.x / 2 + dim.x / 2;
                pos.x += 5;
            }

            objects[i].position.x = pos.x;
        }
    }
};


#endif //V_3D_SCENE_H
