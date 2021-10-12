//
// Created by iXasthur on 12.10.2021.
//

#ifndef TAMARA_3D_VERTEX_H
#define TAMARA_3D_VERTEX_H

#include "../../../utils/Vector3.h"
#include "../../../utils/Matrix4.h"

class Vertex {
public:
    Vector3 position;
    Vector3 normal;

    Vertex(const Vector3 &position, const Vector3 &normal) : position(position), normal(normal) {}

    Vertex() = default;

    [[nodiscard]] Vertex matrixMultiplied(const Matrix4 &m) const {
        return {Matrix4::multiplyVector(position, m), normal};
    }
};


#endif //TAMARA_3D_VERTEX_H
