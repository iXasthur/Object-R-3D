//
// Created by iXasthur on 16.11.2020.
//

#ifndef V_3D_LIGHT_H
#define V_3D_LIGHT_H

#include <vector>
#include "../../utils/Vector4.h"

class DirectionalLight {
public:
    Vector3 direction;

    DirectionalLight(Vector3 dir) : direction(dir) {

    }
};

#endif //V_3D_LIGHT_H
