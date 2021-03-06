//
// Created by iXasthur on 14.11.2020.
//

#ifndef OBJECT_R_3D_CAMERA_H
#define OBJECT_R_3D_CAMERA_H

#include "../../utils/Vector3.h"
#include "../../utils/EulerAngle.h"

class Camera {
private:
    Vector3 iUpVector = {0, 1, 0};
    Vector3 iTargetVector = {0, 0, -1};

public:
    float fFOV = 90.0f;
    float fFar = 100.0f;
    float fNear = 0.1f;

    Vector3 position;
    EulerAngle eulerRotation;

    Camera(const Vector3 &position, const EulerAngle &eulerRotation) {
        this->position = position;
        this->eulerRotation = eulerRotation;
    }

    [[nodiscard]] Vector3 getInitialUpVector() const {
        return this->iUpVector;
    }

    [[nodiscard]] Vector3 getInitialTargetVector() const {
        return this->iTargetVector;
    }
};


#endif //OBJECT_R_3D_CAMERA_H
