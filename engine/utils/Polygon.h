//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_MYPOLYGON_H
#define V_3D_MYPOLYGON_H

#include <array>
#include "Vector3.h"

class Polygon {
public:
    std::array<Vector3, 3> vertices;
    std::array<Vector3, 3> normals;

    explicit Polygon(std::array<Vector3, 3> vertices) {
        this->vertices = vertices;
        for (auto &normal : normals) {
            normal = getNormal();
        }
    }

    Polygon(Vector3 v0, Vector3 v1, Vector3 v2) {
        std::array<Vector3, 3> array = {v0, v1, v2};
        this->vertices = array;
        for (auto &normal : normals) {
            normal = getNormal();
        }
    }

    Polygon(std::array<Vector3, 3> vertices, std::array<Vector3, 3> normals) {
        this->vertices = vertices;
        this->normals = normals;
    }

    Vector3 getNormal() {
        Vector3 a = vertices[0] - vertices[1];
        Vector3 b = vertices[0] - vertices[2];
        return Vector3::cross(a, b);
    }
};


#endif //V_3D_MYPOLYGON_H
