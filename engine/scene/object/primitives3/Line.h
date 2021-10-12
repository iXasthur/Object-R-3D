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

    [[nodiscard]] float getXtYZ(float targetY, float targetZ) const {
        Vector3 r = Vector3::sub(v1.position, v0.position);

        if (r.y != 0.0f) {
            float t = (targetY - v0.position.y) / r.y;
            return v0.position.x + t * r.x;
        }

        if (r.z != 0.0f) {
            float t = (targetZ - v0.position.z) / r.z;
            return v0.position.x + t * r.x;
        }

        std::cout << "getXtYZ == nan" << std::endl;

        return std::numeric_limits<float>::quiet_NaN();
    }

    [[nodiscard]] float getYtXZ(float targetX, float targetZ) const {
        Vector3 r = Vector3::sub(v1.position, v0.position);

        if (r.x != 0.0f) {
            float t = (targetX - v0.position.x) / r.x;
            return v0.position.y + t * r.y;
        }

        if (r.z != 0.0f) {
            float t = (targetZ - v0.position.z) / r.z;
            return v0.position.y + t * r.y;
        }

        std::cout << "getYtXZ == nan" << std::endl;

        return std::numeric_limits<float>::quiet_NaN();
    }

    [[nodiscard]] float getZtXY(float targetX, float targetY) const {
        Vector3 r = Vector3::sub(v1.position, v0.position);

        if (r.x != 0.0f) {
            float t = (targetX - v0.position.x) / r.x;
            return v0.position.z + t * r.z;
        }

        if (r.y != 0.0f) {
            float t = (targetY - v0.position.y) / r.y;
            return v0.position.z + t * r.z;
        }

        std::cout << "getZtXY == nan" << std::endl;

        return std::numeric_limits<float>::quiet_NaN();
    }

    [[nodiscard]] Vector3 getInterpolatedNormalXY(float x, float y) const {
        Vector3 n0 = v0.normal;
        Vector3 n1 = v1.normal;

        float y0 = v0.position.y;
        float y1 = v1.position.y;
        float x0 = v0.position.x;
        float x1 = v1.position.x;

        Vector3 in0;
        Vector3 in1;
        if (y0 != y1) {
            in0 = Vector3::mul(n0, (y - y1) / (y0 - y1));
            in1 = Vector3::mul(n1, (y0 - y) / (y0 - y1));
        } else if (x0 != x1) {
            in0 = Vector3::mul(n0, (x - x1) / (x0 - x1));
            in1 = Vector3::mul(n1, (x0 - x) / (x0 - x1));
        } else {
            std::cout << "getInterpolatedNormalXY == nan" << std::endl;
            return Vector3::nan();
        }

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
