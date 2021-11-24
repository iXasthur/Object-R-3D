//
// Created by iXasthur on 12.10.2021.
//

#ifndef OBJECT_R_3D_LINE_H
#define OBJECT_R_3D_LINE_H

#include "Vertex.h"
#include "Plane.h"

class Line {
public:
    Vertex v0;
    Vertex v1;

    Line(const Vertex &v0, const Vertex &v1) : v0(v0), v1(v1) {}

//    [[nodiscard]] Vertex intersectPlane(const Plane &plane) const {
//        Vector3 plane_n = Vector3::normalize(plane_n);
//        Vector3 plane_p = plane.p;
//        Vector3 lineStart = v0.position;
//        Vector3 lineEnd = v1.position;
//
//        float plane_d = Vector3::dotProduct(plane_n, plane_p);
//        float ad = Vector3::dotProduct(lineStart, plane_n);
//        float bd = Vector3::dotProduct(lineEnd, plane_n);
//        float t = (plane_d - ad) / (bd - ad);
//        Vector3 lineStartToEnd = Vector3::sub(lineEnd, lineStart);
//        Vector3 lineToIntersect = Vector3::mul(lineStartToEnd, t);
//
//        Vector3 position = Vector3::add(lineStart, lineToIntersect);
//        Vector3 normal = getInterpolatedNormal(position.x, position.y, position.z);
//        Vector3 texture = getInterpolatedTexture(position.x, position.y, position.z, false);
//        return {position, texture, normal};
//    }

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

        return std::numeric_limits<float>::quiet_NaN();
    }

    [[nodiscard]] Vector3 getInterpolatedNormal(float x, float y, float z) const {
        return getInterpolated(v0.normal, v1.normal, x, y, z, false, {}, {});
    }

    [[nodiscard]] Vector3 getInterpolatedTexture(float x, float y, float z, bool perspectiveCorrect, const Matrix4 &matProj_inverse, const Matrix4 &matScreen_inverse) const {
        Vector3 interpolated = getInterpolated(v0.texture, v1.texture, x, y, z, perspectiveCorrect, matProj_inverse, matScreen_inverse);
        float *components[2] = {&interpolated.x, &interpolated.y};
        for (auto &component: components) {
            if (*component > 1) *component = 1;
            if (*component < 0) *component = 0;
        }
        return interpolated;
    }

    [[nodiscard]] Vector3 getInterpolated(const Vector3 &arg0, const Vector3 &arg1, float x, float y, float z, bool perspectiveCorrect, const Matrix4 &matProj_inverse, const Matrix4 &matScreen_inverse) const {
        float x0 = v0.position.x;
        float x1 = v1.position.x;
        float y0 = v0.position.y;
        float y1 = v1.position.y;
        float z0 = v0.position.z;
        float z1 = v1.position.z;

        float t;
        float q;
        if (y0 != y1) {
            t = (y - y1) / (y0 - y1);
            q = (y0 - y) / (y0 - y1);
        } else if (x0 != x1) {
            t = (x - x1) / (x0 - x1);
            q = (x0 - x) / (x0 - x1);
        } else if (z0 != z1) {
            t = (z - z1) / (z0 - z1);
            q = (z0 - z) / (z0 - z1);
        } else {
            return Vector3::nan();
        }

        Vector3 interpolated;

        if (perspectiveCorrect) {
            Vector3 convertedV0 = v0.position;
            convertedV0 = Matrix4::multiplyVector(convertedV0, matScreen_inverse);
            convertedV0 = Matrix4::multiplyVector(convertedV0, matProj_inverse);

            Vector3 convertedV1 = v1.position;
            convertedV1 = Matrix4::multiplyVector(convertedV1, matScreen_inverse);
            convertedV1 = Matrix4::multiplyVector(convertedV1, matProj_inverse);

            float z0_conv = convertedV0.z;
            float z1_conv = convertedV1.z;

            Vector3 in0 = Vector3::mul(Vector3::div(arg0, z0_conv), t);
            Vector3 in1 = Vector3::mul(Vector3::div(arg1, z1_conv), q);
            interpolated = Vector3::div(Vector3::add(in0, in1), (t / z0_conv) + (q / z1_conv));
        } else {
            Vector3 in0 = Vector3::mul(arg0, t);
            Vector3 in1 = Vector3::mul(arg1, q);
            interpolated = Vector3::add(in0, in1);
        }

        return interpolated;
    }
};


#endif //OBJECT_R_3D_LINE_H
