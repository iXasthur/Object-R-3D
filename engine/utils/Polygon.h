//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_MYPOLYGON_H
#define V_3D_MYPOLYGON_H

#include <array>
#include "Vector3.h"
#include "Color.h"
#include "Matrix4.h"

class Polygon {
private:
    Vector3 getNormalByVertex(Vector3 &vertex) {
        for (int i = 0; i < 3; ++i) {
            if (Vector3::equals(vertices[i], vertex)) {
                return normals[i];
            }
        }

        return {0, 1, 0};
    }

public:
    std::array<Vector3, 3> vertices{};
    std::array<Vector3, 3> normals{};

    Color color = Color(255, 255, 255, 255);

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

    Polygon matrixMultiplied(Matrix4 &mx) {
        return {
            Matrix4::multiplyVector(vertices[0], mx),
            Matrix4::multiplyVector(vertices[1], mx),
            Matrix4::multiplyVector(vertices[2], mx)
        };
    }

    [[nodiscard]] bool isFlat() const {
        Polygon roundedPolygon = *this;
        for (int i = 0; i < 3; i++) {
            roundedPolygon.vertices[i].x = std::floor(roundedPolygon.vertices[i].x);
            roundedPolygon.vertices[i].y = std::floor(roundedPolygon.vertices[i].y);
        }

        if (roundedPolygon.vertices[0].y == roundedPolygon.vertices[1].y &&
            roundedPolygon.vertices[1].y == roundedPolygon.vertices[2].y) {
            return true;
        }

        if (roundedPolygon.vertices[0].x == roundedPolygon.vertices[1].x &&
            roundedPolygon.vertices[1].x == roundedPolygon.vertices[2].x) {
            return true;
        }

        return false;
    }

    std::vector<Polygon> clipAgainstPlane(Vector3 plane_p, Vector3 plane_n) {
        // Make sure plane normal is indeed normal
        plane_n = Vector3::normalize(plane_n);

        // Return signed shortest distance from point to plane, plane normal must be normalised
        auto dist = [&](Vector3 &p)
        {
            return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector3::dotProduct(plane_n, plane_p));
        };

        // Create two temporary storage arrays to classify points either side of plane
        // If distance sign is positive, point lies on "inside" of plane
        Vector3 *inside_points[3];  int nInsidePointCount = 0;
        Vector3 *outside_points[3]; int nOutsidePointCount = 0;

        // Get signed distance of each point in triangle to plane
        float d0 = dist(this->vertices[0]);
        float d1 = dist(this->vertices[1]);
        float d2 = dist(this->vertices[2]);

        if (d0 >= 0) { inside_points[nInsidePointCount++] = &this->vertices[0]; }
        else { outside_points[nOutsidePointCount++] = &this->vertices[0]; }
        if (d1 >= 0) { inside_points[nInsidePointCount++] = &this->vertices[1]; }
        else { outside_points[nOutsidePointCount++] = &this->vertices[1]; }
        if (d2 >= 0) { inside_points[nInsidePointCount++] = &this->vertices[2]; }
        else { outside_points[nOutsidePointCount++] = &this->vertices[2]; }

        // Now classify triangle points, and break the input triangle into
        // smaller output triangles if required. There are four possible
        // outcomes...

        if (nInsidePointCount == 0)
        {
            // All points lie on the outside of plane, so clip whole triangle
            // It ceases to exist

            return {}; // No returned triangles are valid
        }

        if (nInsidePointCount == 3)
        {
            // All points lie on the inside of plane, so do nothing
            // and allow the triangle to simply pass through

            Polygon out = *this;
            return {out}; // Just the one returned original triangle is valid
        }

        if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
            // Triangle should be clipped. As two points lie outside
            // the plane, the triangle simply becomes a smaller triangle

            Polygon out = *this;

            out.vertices = {
                    // The inside point is valid, so keep that...
                    *inside_points[0],
                    // but the two new points are at the locations where the
                    // original sides of the triangle (lines) intersect with the plane
                    Vector3::intersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]),
                    Vector3::intersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1])
            };

            out.normals = {
                    getNormalByVertex(*inside_points[0]),
                    getNormalByVertex(*outside_points[0]),
                    getNormalByVertex(*outside_points[1])
            };

            return {out}; // Return the newly formed single triangle
        }

        if (nInsidePointCount == 2 && nOutsidePointCount == 1)
        {
            // Triangle should be clipped. As two points lie inside the plane,
            // the clipped triangle becomes a "quad". Fortunately, we can
            // represent a quad with two new triangles

            Polygon out0 = *this;
            Polygon out1 = *this;

            // The first triangle consists of the two inside points and a new
            // point determined by the location where one side of the triangle
            // intersects with the plane
            out0.vertices = {
                    *inside_points[0],
                    *inside_points[1],
                    Vector3::intersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0])
            };

            out0.normals = {
                    getNormalByVertex(*inside_points[0]),
                    getNormalByVertex(*inside_points[1]),
                    getNormalByVertex(*outside_points[0])
            };

            // The second triangle is composed of one of he inside points, a
            // new point determined by the intersection of the other side of the
            // triangle and the plane, and the newly created point above
            out1.vertices = {
                    *inside_points[1],
                    out0.vertices[2],
                    Vector3::intersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0])
            };

            out1.normals = {
                    getNormalByVertex(*inside_points[1]),
                    out0.normals[2],
                    getNormalByVertex(*outside_points[0])
            };

            return {out0, out1}; // Return two newly formed triangles which form a quad
        }

        return {};
    }
};


#endif //V_3D_MYPOLYGON_H
