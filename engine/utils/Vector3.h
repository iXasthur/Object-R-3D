//
// Created by iXasthur on 15.11.2020.
//

#ifndef OBJECT_R_3D_VECTOR3_H
#define OBJECT_R_3D_VECTOR3_H

#include <array>
#include <cmath>
#include <string>

class Vector3 {
public:
    float x;
    float y;
    float z;

    Vector3(int x, int y, int z) : x((float) x), y((float) y), z((float) z) {

    }

    Vector3(float x, float y, float z) : x(x), y(y), z(z) {

    }

    Vector3() : x(0), y(0), z(0) {

    }

    std::array<float, 3> toArray() {
        return {x, y, z};
    }

    [[nodiscard]] std::string toString() const {
        std::string xx = std::to_string(x);
        xx = xx.substr(0, xx.find('.') + 3);

        std::string xy = std::to_string(y);
        xy = xy.substr(0, xx.find('.') + 3);

        std::string xz = std::to_string(z);
        xz = xz.substr(0, xz.find('.') + 3);

        return "x: " + xx + " y: " + xy + " z: " + xz;
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

    static Vector3 nan() {
        float nan = std::numeric_limits<float>::quiet_NaN();
        return {nan, nan, nan};
    }

};


#endif //OBJECT_R_3D_VECTOR3_H
