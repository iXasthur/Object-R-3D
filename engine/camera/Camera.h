//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_CAMERA_H
#define V_3D_CAMERA_H

#include "../utils/Vector3.h"
#include "../utils/EulerAngle.h"

class Camera {
public:
    const float fFOV = 90.0f;
    const float fFar = 1000.0f;
    const float fNear = 0.1f;

    Vector3 position;
    EulerAngle eulerRotation;

    explicit Camera(Vector3 position) {
        this->position = position;
        this->eulerRotation = EulerAngle();
    }
};


#endif //V_3D_CAMERA_H
