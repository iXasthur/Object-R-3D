//
// Created by iXasthur on 15.11.2020.
//

#ifndef V_3D_VECTOR3_H
#define V_3D_VECTOR3_H

#include <array>
#include <cmath>

class Vector3 {
public:
    float x;
    float y;
    float z;

    Vector3(float x, float y, float z) : x(x), y(y), z(z) {

    }

    Vector3() : x(0), y(0), z(0) {

    }

    std::array<float, 3> toArray() {
        return {x, y, z};
    }

    [[nodiscard]] std::string toString() const {
        std::string s;

        std::string xs = std::to_string(x);
        xs = xs.substr(0, xs.find('.') + 3);

        std::string xy = std::to_string(y);
        xy = xy.substr(0, xs.find('.') + 3);

        std::string xz = std::to_string(z);
        xz = xz.substr(0, xz.find('.') + 3);

        s = "x: " + xs + " y: " + xy + " z: " + xz;
        return s;
    }


    // ---- 3D-GE LIB ----

    static Vector3 add(const Vector3 &v1, const Vector3 &v2) {
        return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    }

    static Vector3 sub(const Vector3 &v1, const Vector3 &v2) {
        return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
    }

    static Vector3 mul(const Vector3 &v1, float k) {
        return { v1.x * k, v1.y * k, v1.z * k };
    }

    static Vector3 div(const Vector3 &v1, float k) {
        return { v1.x / k, v1.y / k, v1.z / k };
    }

    static float dotProduct(const Vector3 &v1, const Vector3 &v2) {
        return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    }

    static float length(const Vector3 &v) {
        return sqrtf(Vector3::dotProduct(v, v));
    }

    static bool equals(const Vector3 &v1, const Vector3 &v2) {
        if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z){
            return true;
        }
        return false;
    }

    static Vector3 normalize(const Vector3 &v) {
        float l = Vector3::length(v);
        return { v.x / l, v.y / l, v.z / l };
    }

    static Vector3 crossProduct(const Vector3 &v1, const Vector3 &v2) {
        Vector3 v;
        v.x = v1.y * v2.z - v1.z * v2.y;
        v.y = v1.z * v2.x - v1.x * v2.z;
        v.z = v1.x * v2.y - v1.y * v2.x;
        return v;
    }

    static Vector3 intersectPlane(const Vector3 &plane_p, const Vector3 &plane_n, const Vector3 &lineStart, const Vector3 &lineEnd) {
        Vector3 pn = Vector3::normalize(plane_n);
        float plane_d = -Vector3::dotProduct(pn, plane_p);
        float ad = Vector3::dotProduct(lineStart, pn);
        float bd = Vector3::dotProduct(lineEnd, pn);
        float t = (-plane_d - ad) / (bd - ad);
        Vector3 lineStartToEnd = Vector3::sub(lineEnd, lineStart);
        Vector3 lineToIntersect = Vector3::mul(lineStartToEnd, t);
        return Vector3::add(lineStart, lineToIntersect);
    }

    static float getLineZtX(const Vector3 &linePoint0, const Vector3 &linePoint1, float targetX) {
        Vector3 r = Vector3::sub(linePoint1, linePoint0);
        float t = (targetX - linePoint0.x) / r.x;
        return linePoint0.z + t * r.z;
    }

    static float getLineXtY(const Vector3 &linePoint0, const Vector3 &linePoint1, float targetY) {
        Vector3 r = Vector3::sub(linePoint1, linePoint0);
        float t = (targetY - linePoint0.y) / r.y;
        return linePoint0.x + t * r.x;
    }
};


#endif //V_3D_VECTOR3_H
