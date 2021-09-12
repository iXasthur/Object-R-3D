//
// Created by iXasthur on 15.11.2020.
//

#ifndef V_3D_EULERANGLE_H
#define V_3D_EULERANGLE_H


class EulerAngle {
public:
    float x;
    float y;
    float z;

    EulerAngle(float x, float y, float z) : x(x), y(y), z(z) {

    }

    EulerAngle() : x(0), y(0), z(0) {

    }
};


#endif //V_3D_EULERANGLE_H
