//
// Created by iXasthur on 12.10.2021.
//

#ifndef OBJECT_R_3D_VERTEX_H
#define OBJECT_R_3D_VERTEX_H

#include "../../../utils/Vector3.h"
#include "../../../utils/Matrix4.h"

class Vertex {
public:
    Vector3 position;
    Vector3 texture;
    Vector3 normal;

    Vertex(const Vector3 &position, const Vector3 &texture, const Vector3 &normal) : position(position), texture(texture), normal(normal) {}

    Vertex() = default;

    [[nodiscard]] Vertex matrixMultiplied(const Matrix4 &m) const {
        return {Matrix4::multiplyVector(position, m), texture, normal};
    }
};


#endif //OBJECT_R_3D_VERTEX_H
