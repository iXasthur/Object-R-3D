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

    [[nodiscard]] Vertex intersectPlane(const Plane &plane) const {
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

        Vector3 position = Vector3::add(lineStart, lineToIntersect);
        Vector3 normal = getInterpolatedNormal(position.x, position.y, position.z);
        Vector3 texture = getInterpolatedTexture(position.x, position.y, position.z);
        return {position, texture, normal};
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
        return getInterpolated(v0.normal, v1.normal, x, y, z);
    }

    [[nodiscard]] Vector3 getInterpolatedTexture(float x, float y, float z) const {
        return getInterpolated(v0.texture, v1.texture, x, y, z);
    }

    [[nodiscard]] Vector3 getInterpolated(const Vector3 &arg0, const Vector3 &arg1, float x, float y, float z) const {
        float x0 = v0.position.x;
        float x1 = v1.position.x;
        float y0 = v0.position.y;
        float y1 = v1.position.y;
        float z0 = v0.position.z;
        float z1 = v1.position.z;

        Vector3 in0;
        Vector3 in1;
        if (y0 != y1) {
            in0 = Vector3::mul(arg0, (y - y1) / (y0 - y1));
            in1 = Vector3::mul(arg1, (y0 - y) / (y0 - y1));
        } else if (x0 != x1) {
            in0 = Vector3::mul(arg0, (x - x1) / (x0 - x1));
            in1 = Vector3::mul(arg1, (x0 - x) / (x0 - x1));
        } else if (z0 != z1) {
            in0 = Vector3::mul(arg0, (z - z1) / (z0 - z1));
            in1 = Vector3::mul(arg1, (z0 - z) / (z0 - z1));
        } else {
            return Vector3::nan();
        }

        return Vector3::add(in0, in1);
    }
};


#endif //TAMARA_3D_LINE_H
