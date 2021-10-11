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
public:
    std::array<Vector3, 3> vertices{};
    std::array<Vector3, 3> normals{};

    explicit Polygon(const std::array<Vector3, 3> &vertices) {
        this->vertices = vertices;
        for (auto &normal : normals) {
            normal = getNormal();
        }
    }

    Polygon(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2) {
        std::array<Vector3, 3> array = {v0, v1, v2};
        this->vertices = array;
        for (auto &normal : normals) {
            normal = getNormal();
        }
    }

    Polygon(const std::array<Vector3, 3> &vertices, const std::array<Vector3, 3> &normals) {
        this->vertices = vertices;
        this->normals = normals;
    }

    [[nodiscard]] Polygon getSortedY() const {
        Polygon polygon = *this;

        int rc = 0;
        while (rc < 3) {
            if (polygon.vertices[1].y <= polygon.vertices[0].y && polygon.vertices[1].y >= polygon.vertices[2].y) {
                return polygon;
            }
            if (polygon.vertices[1].y >= polygon.vertices[0].y && polygon.vertices[1].y <= polygon.vertices[2].y) {
                return polygon;
            }

            polygon = polygon.getRotatedClockwise();
            rc++;
        }

        throw std::invalid_argument("Unable to create Y sorted polygon");
    }

    // 0 is top
    // 1 is bottom
    [[nodiscard]] std::array<Polygon, 2> splitHorizontally() const {
        Polygon ySorted = getSortedY();

        Vector3 splitPointVertex;
        Vector3 splitPointNormal;
        splitPointVertex.y = ySorted.vertices[1].y;
        splitPointVertex.x = Vector3::getLineXtY(ySorted.vertices[0], ySorted.vertices[2], splitPointVertex.y);
        splitPointVertex.z = Vector3::getLineZtX(ySorted.vertices[0], ySorted.vertices[2], splitPointVertex.x);
        splitPointNormal = Vector3::getInterpolatedNormalY(
                ySorted.vertices[0],
                ySorted.vertices[2],
                ySorted.normals[0],
                ySorted.normals[2],
                splitPointVertex.y
                );

        std::array<Vector3, 3> vTop;
        std::array<Vector3, 3> nTop;
        std::array<Vector3, 3> vBottom;
        std::array<Vector3, 3> nBottom;
        if (ySorted.vertices[1].x < splitPointVertex.x) {
            vTop = {ySorted.vertices[0], splitPointVertex, ySorted.vertices[1]};
            nTop = {ySorted.normals[0], splitPointNormal, ySorted.normals[1]};
            vBottom = {ySorted.vertices[2], ySorted.vertices[1], splitPointVertex};
            nBottom = {ySorted.normals[2], ySorted.normals[1], splitPointNormal};
        } else {
            vTop = {ySorted.vertices[1], splitPointVertex, ySorted.vertices[0]};
            nTop = {ySorted.normals[1], splitPointNormal, ySorted.normals[0]};
            vBottom = {ySorted.vertices[1], ySorted.vertices[2], splitPointVertex};
            nBottom = {ySorted.normals[1], ySorted.normals[2], splitPointNormal};
        }

        Polygon pTop = {vTop, nTop};
        Polygon pBottom = {vBottom, nBottom};

        return {pTop, pBottom};
    }

    [[nodiscard]] Polygon getRotatedClockwise() const {
        std::array<Vector3, 3> v = {vertices[2], vertices[0], vertices[1]};
        std::array<Vector3, 3> n = {normals[2], normals[0], normals[1]};
        return {v, n};
    }

    [[nodiscard]] bool isBottomFlat() const {
        std::array<Vector3, 3> vSorted = vertices;
        std::sort(vSorted.begin(), vSorted.end(), [&](const Vector3 &v0, const Vector3 &v1) {
            return v0.y < v1.y;
        });

        return std::floor(vSorted[1].y) == std::floor(vSorted[2].y);
    }

    [[nodiscard]] bool isTopFlat() const {
        std::array<Vector3, 3> vSorted = vertices;
        std::sort(vSorted.begin(), vSorted.end(), [&](const Vector3 &v0, const Vector3 &v1) {
            return v0.y < v1.y;
        });

        return std::floor(vSorted[0].y) == std::floor(vSorted[1].y);
    }

    [[nodiscard]] Vector3 getNormalByVertex(const Vector3 &vertex) const {
        for (int i = 0; i < 3; ++i) {
            if (Vector3::equals(vertices[i], vertex)) {
                return normals[i];
            }
        }

        throw std::invalid_argument("Received invalid vertex");
    }

    [[nodiscard]] Vector3 getNormal() const {
        Vector3 a = Vector3::sub(vertices[0], vertices[1]);
        Vector3 b = Vector3::sub(vertices[0], vertices[2]);
        return Vector3::crossProduct(a, b);
    }

    [[nodiscard]] Polygon matrixMultiplied(const Matrix4 &mx) const {
        return {
                {
                        Matrix4::multiplyVector(vertices[0], mx),
                        Matrix4::multiplyVector(vertices[1], mx),
                        Matrix4::multiplyVector(vertices[2], mx)
                },
                normals
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

//    [[nodiscard]] std::vector<Polygon> clipAgainstPlane(const Vector3 &plane_p, const Vector3 &plane_n) const {
//        // Make sure plane normal is indeed normal
//        Vector3 pn = Vector3::normalize(plane_n);
//
//        // Return signed shortest distance from point to plane, plane normal must be normalised
//        auto dist = [&](const Vector3 &p)
//        {
//            return (pn.x * p.x + pn.y * p.y + pn.z * p.z - Vector3::dotProduct(pn, plane_p));
//        };
//
//        // Create two temporary storage arrays to classify points either side of plane
//        // If distance sign is positive, point lies on "inside" of plane
//        const Vector3 *inside_points[3];  int nInsidePointCount = 0;
//        const Vector3 *outside_points[3]; int nOutsidePointCount = 0;
//
//        // Get signed distance of each point in triangle to plane
//        float d0 = dist(this->vertices[0]);
//        float d1 = dist(this->vertices[1]);
//        float d2 = dist(this->vertices[2]);
//
//        if (d0 >= 0) { inside_points[nInsidePointCount++] = &this->vertices[0]; }
//        else { outside_points[nOutsidePointCount++] = &this->vertices[0]; }
//        if (d1 >= 0) { inside_points[nInsidePointCount++] = &this->vertices[1]; }
//        else { outside_points[nOutsidePointCount++] = &this->vertices[1]; }
//        if (d2 >= 0) { inside_points[nInsidePointCount++] = &this->vertices[2]; }
//        else { outside_points[nOutsidePointCount++] = &this->vertices[2]; }
//
//        // Now classify triangle points, and break the input triangle into
//        // smaller output triangles if required. There are four possible
//        // outcomes...
//
//        if (nInsidePointCount == 0)
//        {
//            // All points lie on the outside of plane, so clip whole triangle
//            // It ceases to exist
//
//            return {}; // No returned triangles are valid
//        }
//
//        if (nInsidePointCount == 3)
//        {
//            // All points lie on the inside of plane, so do nothing
//            // and allow the triangle to simply pass through
//
//            Polygon out = *this;
//            return {out}; // Just the one returned original triangle is valid
//        }
//
//        if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
//            // Triangle should be clipped. As two points lie outside
//            // the plane, the triangle simply becomes a smaller triangle
//
//            Polygon out = *this;
//
//            out.vertices = {
//                    // The inside point is valid, so keep that...
//                    *inside_points[0],
//                    // but the two new points are at the locations where the
//                    // original sides of the triangle (lines) intersect with the plane
//                    Vector3::intersectPlane(plane_p, pn, *inside_points[0], *outside_points[0]),
//                    Vector3::intersectPlane(plane_p, pn, *inside_points[0], *outside_points[1])
//            };
//
//            out.normals = {
//                    getInterpolatedNormal(out.vertices[0]),
//                    getInterpolatedNormal(out.vertices[1]),
//                    getInterpolatedNormal(out.vertices[2]),
//            };
//
//            return {out}; // Return the newly formed single triangle
//        }
//
//        if (nInsidePointCount == 2 && nOutsidePointCount == 1)
//        {
//            // Triangle should be clipped. As two points lie inside the plane,
//            // the clipped triangle becomes a "quad". Fortunately, we can
//            // represent a quad with two new triangles
//
//            Polygon out0 = *this;
//            Polygon out1 = *this;
//
//            // The first triangle consists of the two inside points and a new
//            // point determined by the location where one side of the triangle
//            // intersects with the plane
//            out0.vertices = {
//                    *inside_points[0],
//                    *inside_points[1],
//                    Vector3::intersectPlane(plane_p, pn, *inside_points[0], *outside_points[0])
//            };
//
//            out0.normals = {
//                    getInterpolatedNormal(out0.vertices[0]),
//                    getInterpolatedNormal(out0.vertices[1]),
//                    getInterpolatedNormal(out0.vertices[2]),
//            };
//
//            // The second triangle is composed of one of he inside points, a
//            // new point determined by the intersection of the other side of the
//            // triangle and the plane, and the newly created point above
//            out1.vertices = {
//                    *inside_points[1],
//                    out0.vertices[2],
//                    Vector3::intersectPlane(plane_p, pn, *inside_points[1], *outside_points[0])
//            };
//
//            out1.normals = {
//                    getInterpolatedNormal(out1.vertices[0]),
//                    getInterpolatedNormal(out1.vertices[1]),
//                    getInterpolatedNormal(out1.vertices[2]),
//            };
//
//            return {out0, out1}; // Return two newly formed triangles which form a quad
//        }
//
//        return {};
//    }
};


#endif //V_3D_MYPOLYGON_H
