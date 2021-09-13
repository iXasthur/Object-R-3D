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

    Vector3 operator-(const Vector3 &b) const {
        Vector3 vector;
        vector.x = this->x - b.x;
        vector.y = this->y - b.y;
        vector.z = this->z - b.z;
        return vector;
    }

    Vector3 operator+(const Vector3 &b) const {
        Vector3 vector;
        vector.x = this->x + b.x;
        vector.y = this->y + b.y;
        vector.z = this->z + b.z;
        return vector;
    }

    std::array<float, 3> toArray() {
        return {x, y, z};
    }

    [[nodiscard]] float length() const {
        return std::sqrt((x * x) + (y * y) + (z * z));
    }

    void normalize() {
        float length = this->length();
        x = x / length;
        y = y / length;
        z = z / length;
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

    static Vector3 cross(Vector3 v1, Vector3 v2) {
        Vector3 v;
        v.x = v1.y * v2.z - v1.z * v2.y;
        v.y = v1.z * v2.x - v1.x * v2.z;
        v.z = v1.x * v2.y - v1.y * v2.x;
        return v;
    }


    // ---- 3D-GE LIB ----

    static Vector3 add(Vector3 &v1, Vector3 &v2) {
        return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    }

    static Vector3 sub(Vector3 &v1, Vector3 &v2) {
        return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
    }

    static Vector3 mul(Vector3 &v1, float k) {
        return { v1.x * k, v1.y * k, v1.z * k };
    }

    static Vector3 div(Vector3 &v1, float k) {
        return { v1.x / k, v1.y / k, v1.z / k };
    }

    static float dotProduct(Vector3 &v1, Vector3 &v2) {
        return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    }

    static float length(Vector3 &v)
    {
        return sqrtf(Vector3::dotProduct(v, v));
    }

    static bool equals(Vector3 &v1, Vector3 &v2) {
        if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z){
            return true;
        }
        return false;
    }

    static Vector3 normalize(Vector3 &v) {
        float l = Vector3::length(v);
        return { v.x / l, v.y / l, v.z / l };
    }

    static Vector3 crossProduct(Vector3 &v1, Vector3 &v2)
    {
        Vector3 v;
        v.x = v1.y * v2.z - v1.z * v2.y;
        v.y = v1.z * v2.x - v1.x * v2.z;
        v.z = v1.x * v2.y - v1.y * v2.x;
        return v;
    }

    static Vector3 intersectPlane(Vector3 &plane_p, Vector3 &plane_n, Vector3 &lineStart, Vector3 &lineEnd)
    {
        plane_n = Vector3::normalize(plane_n);
        float plane_d = -Vector3::dotProduct(plane_n, plane_p);
        float ad = Vector3::dotProduct(lineStart, plane_n);
        float bd = Vector3::dotProduct(lineEnd, plane_n);
        float t = (-plane_d - ad) / (bd - ad);
        Vector3 lineStartToEnd = Vector3::sub(lineEnd, lineStart);
        Vector3 lineToIntersect = Vector3::mul(lineStartToEnd, t);
        return Vector3::add(lineStart, lineToIntersect);
    }
};


#endif //V_3D_VECTOR3_H
