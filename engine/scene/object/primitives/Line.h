//
// Created by iXasthur on 12.10.2021.
//

#ifndef TAMARA_3D_LINE_H
#define TAMARA_3D_LINE_H

#include "Vertex.h"
#include "Plane.h"

class Line {
public:
    Vertex v0;
    Vertex v1;

    Line(const Vertex &v0, const Vertex &v1) : v0(v0), v1(v1) {}

    [[nodiscard]] Vector3 intersectPlane(const Plane &plane) const {
        Vector3 plane_n = Vector3::normalize(plane_n);
        Vector3 plane_p = plane.p;
        Vector3 lineStart = v0.position;
        Vector3 lineEnd = v1.position;

        float plane_d = Vector3::dotProduct(plane_n, plane_p);
        float ad = Vector3::dotProduct(lineStart, plane_n);
        float bd = Vector3::dotProduct(lineEnd, plane_n);
        float t = (plane_d - ad) / (bd - ad);
        Vector3 lineStartToEnd = Vector3::sub(lineEnd, lineStart);
        Vector3 lineToIntersect = Vector3::mul(lineStartToEnd, t);
        return Vector3::add(lineStart, lineToIntersect);
    }

    [[nodiscard]] float getLineZtX(float targetX) const {
        Vector3 r = Vector3::sub(v1.position, v0.position);
        float t = (targetX - v0.position.x) / r.x; // FIXME
        return v0.position.z + t * r.z;
    }

    [[nodiscard]] float getLineXtY(float targetY) const {
        Vector3 r = Vector3::sub(v1.position, v0.position);
        float t = (targetY - v0.position.y) / r.y; // FIXME
        return v0.position.x + t * r.x;
    }

    [[nodiscard]] Vector3 getInterpolatedNormalY(float y) const {
        Vector3 n0 = v0.normal;
        Vector3 n1 = v1.normal;
        float y0 = v0.position.y;
        float y1 = v1.position.y;

        Vector3 in0 = Vector3::mul(n0, (y - y1) / (y0 - y1)); // FIXME
        Vector3 in1 = Vector3::mul(n1, (y0 - y) / (y0 - y1)); // FIXME
        Vector3 in = Vector3::add(in0, in1);

//        std::cout << "-----" << std::endl;
//        std::cout << n0.toString() << "       ";
//        std::cout << in.toString() << "       ";
//        std::cout << n1.toString() << "       ";
//        std::cout << std::endl;

        return in;
    }
};


#endif //TAMARA_3D_LINE_H
