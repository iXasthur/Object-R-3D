//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_CAMERA_H
#define V_3D_CAMERA_H

#include "../../utils/Vector3.h"
#include "../../utils/EulerAngle.h"

class Camera {
private:
    Vector3 iUpVector = {0, 1, 0};
    Vector3 iTargetVector = {0, 0, 1};

public:
    float fFOV = 90.0f;
    float fFar = 1000.0f;
    float fNear = 1.0f;

    Vector3 position;
    EulerAngle eulerRotation;

    Camera(Vector3 position, EulerAngle eulerRotation) {
        this->position = position;
        this->eulerRotation = eulerRotation;
    }

    Vector3 getInitialUpVector() {
        return this->iUpVector;
    }

    Vector3 getInitialTargetVector() {
        return this->iTargetVector;
    }
};


#endif //V_3D_CAMERA_H
