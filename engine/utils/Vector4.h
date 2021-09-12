//
// Created by iXasthur on 15.11.2020.
//

#ifndef V_3D_VECTOR4_H
#define V_3D_VECTOR4_H

#include <array>

class Vector4 {
public:
    float x;
    float y;
    float z;
    float w;

    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {

    }

    Vector4() : x(0), y(0), z(0), w(0) {

    }

    std::array<float, 4> toArray() {
        return {x, y, z, w};
    }
};

#endif //V_3D_VECTOR4_H
