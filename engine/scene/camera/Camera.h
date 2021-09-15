//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_CAMERA_H
#define V_3D_CAMERA_H

#include "../../utils/Vector3.h"
#include "../../utils/EulerAngle.h"

class Camera {
public:
    float fFOV = 90.0f;
    float fFar = 1000.0f;
    float fNear = 0.1f;

    Vector3 position;
    EulerAngle eulerRotation;

    Camera(Vector3 position, EulerAngle eulerRotation) {
        this->position = position;
        this->eulerRotation = eulerRotation;
    }
};


#endif //V_3D_CAMERA_H
