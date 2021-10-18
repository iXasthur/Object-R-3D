//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_MYPOLYGON_H
#define V_3D_MYPOLYGON_H

#include <array>
#include "../../../utils/Vector3.h"
#include "../../../utils/Color.h"
#include "../../../utils/Matrix4.h"
#include "Vertex.h"
#include "Line.h"
#include "../primitives2/Line2D.h"
#include <SDL2/SDL.h>

class Polygon {
public:
    std::array<Vertex, 3> vertices;

    explicit Polygon(const std::array<Vertex, 3> &vertices) : vertices(vertices) {}

    explicit Polygon(const std::array<Vector3, 3> &positions) : Polygon(positions[0], positions[1], positions[2]) {}

    Polygon(const Vertex &v0, const Vertex &v1, const Vertex &v2) {
        vertices = {v0, v1, v2};
    }

    Polygon(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2) {
        Vector3 fn = Polygon::discoverFaceNormal(p0, p1, p2);
        vertices = {
                Vertex(p0, fn),
                Vertex(p1, fn),
                Vertex(p2, fn),
        };
    }

    Polygon(const std::array<Vector3, 3> &p, const std::array<Vector3, 3> &n) {
        vertices = {
                Vertex(p[0], n[0]),
                Vertex(p[1], n[1]),
                Vertex(p[2], n[2]),
        };
    }

//    [[nodiscard]] Polygon getAlignedY() const {
//        Polygon polygon = *this;
//
//        int rc = 0;
//        while (rc < 3) {
//            if (polygon.vertices[1].position.y <= polygon.vertices[0].position.y && polygon.vertices[1].position.y >= polygon.vertices[2].position.y) {
//                return polygon;
//            }
//            if (polygon.vertices[1].position.y >= polygon.vertices[0].position.y && polygon.vertices[1].position.y <= polygon.vertices[2].position.y) {
//                return polygon;
//            }
//
//            polygon = polygon.getRotatedClockwise();
//            rc++;
//        }
//
//        throw std::invalid_argument("Unable to create Y sorted polygon");
//    }

    // 0 is top
    // 1 is bottom
//    [[nodiscard]] std::array<Polygon, 2> splitHorizontally() const {
//        Line2D sLine2D = {
//                {vertices[0].position.x, vertices[0].position.y},
//                {vertices[1].position.x, vertices[1].position.y}
//        };
//        Line sLine3D = {vertices[0], vertices[1]};
//
//        Polygon ySorted = getAlignedY();
//
//        float splitY = ySorted.vertices[1].position.y;
//        float splitX = sLine2D.getXtY(splitY);
//
//        Vector3 splitPointVertex;
//        Vector3 splitPointNormal;
//        splitPointVertex.x = splitX;
//        splitPointVertex.y = splitY;
//        splitPointVertex.z = sLine3D.getZtXY(splitPointVertex.x, splitPointVertex.y);
//        splitPointNormal = sLine3D.getInterpolatedNormalXY(splitPointVertex.x, splitPointVertex.y);
//
//        std::array<Vector3, 3> vTop;
//        std::array<Vector3, 3> nTop;
//        std::array<Vector3, 3> vBottom;
//        std::array<Vector3, 3> nBottom;
//        if (ySorted.vertices[1].position.x < splitPointVertex.x) {
//            vTop = {ySorted.vertices[0].position, splitPointVertex, ySorted.vertices[1].position};
//            nTop = {ySorted.vertices[0].normal, splitPointNormal, ySorted.vertices[1].normal};
//            vBottom = {ySorted.vertices[2].position, ySorted.vertices[1].position, splitPointVertex};
//            nBottom = {ySorted.vertices[2].normal, ySorted.vertices[1].normal, splitPointNormal};
//        } else {
//            vTop = {ySorted.vertices[1].position, splitPointVertex, ySorted.vertices[0].position};
//            nTop = {ySorted.vertices[1].normal, splitPointNormal, ySorted.vertices[0].normal};
//            vBottom = {ySorted.vertices[1].position, ySorted.vertices[2].position, splitPointVertex};
//            nBottom = {ySorted.vertices[1].normal, ySorted.vertices[2].normal, splitPointNormal};
//        }
//
//        Polygon pTop = {vTop, nTop};
//        Polygon pBottom = {vBottom, nBottom};
//
//        return {pTop, pBottom};
//    }

    [[nodiscard]] Vector3 getCenter() const {
        Vector3 center = Vector3::div(Vector3::add(vertices[0].position, vertices[1].position), 2);
        center = Vector3::div(Vector3::add(center, vertices[2].position), 2);
        return center;
    }

    [[nodiscard]] Polygon getRotatedClockwise() const {
        std::array<Vertex, 3> v = {vertices[2], vertices[0], vertices[1]};
        return Polygon(v);
    }

//    [[nodiscard]] bool isBottomFlat() const {
//        std::array<Vertex, 3> vSorted = vertices;
//        std::sort(vSorted.begin(), vSorted.end(), [&](const Vertex &v0, const Vertex &v1) {
//            return v0.position.y < v1.position.y;
//        });
//        return std::floor(vSorted[1].position.y) == std::floor(vSorted[2].position.y);
//    }
//
//    [[nodiscard]] bool isTopFlat() const {
//        std::array<Vertex, 3> vSorted = vertices;
//        std::sort(vSorted.begin(), vSorted.end(), [&](const Vertex &v0, const Vertex &v1) {
//            return v0.position.y < v1.position.y;
//        });
//        return std::floor(vSorted[0].position.y) == std::floor(vSorted[1].position.y);
//    }

    [[nodiscard]] Vector3 getFaceNormal() const {
        return Polygon::discoverFaceNormal(vertices[0].position, vertices[1].position, vertices[2].position);
    }

    [[nodiscard]] Polygon matrixMultiplied(const Matrix4 &mx) const {
        return {
                vertices[0].matrixMultiplied(mx),
                vertices[1].matrixMultiplied(mx),
                vertices[2].matrixMultiplied(mx)
        };
    }

    [[nodiscard]] bool isFlat() const {
        Polygon roundedPolygon = *this;

        for (int i = 0; i < 3; i++) {
            roundedPolygon.vertices[i].position.x = std::floor(roundedPolygon.vertices[i].position.x);
            roundedPolygon.vertices[i].position.y = std::floor(roundedPolygon.vertices[i].position.y);
        }

        if (roundedPolygon.vertices[0].position.y == roundedPolygon.vertices[1].position.y &&
            roundedPolygon.vertices[1].position.y == roundedPolygon.vertices[2].position.y) {
            return true;
        }

        if (roundedPolygon.vertices[0].position.x == roundedPolygon.vertices[1].position.x &&
            roundedPolygon.vertices[1].position.x == roundedPolygon.vertices[2].position.x) {
            return true;
        }

        return false;
    }

    static Vector3 discoverFaceNormal(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2) {
        Vector3 a = Vector3::sub(p0, p1);
        Vector3 b = Vector3::sub(p0, p2);
        return Vector3::crossProduct(a, b);
    }

    [[nodiscard]] std::vector<Polygon> clipAgainstPlane(const Plane &plane) const {
        // Make sure plane normal is indeed normal
        Vector3 pn = Vector3::normalize(plane.n);

        // Return signed shortest distance from point to plane, plane normal must be normalised
        auto dist = [&](const Vertex &p)
        {
            return (pn.x * p.position.x + pn.y * p.position.y + pn.z * p.position.z - Vector3::dotProduct(pn, plane.p));
        };

        // Create two temporary storage arrays to classify points either side of plane
        // If distance sign is positive, point lies on "inside" of plane
        const Vertex *inside_points[3];  int nInsidePointCount = 0;
        const Vertex *outside_points[3]; int nOutsidePointCount = 0;

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

            Line line0 = {*inside_points[0], *outside_points[0]};
            Line line1 = {*inside_points[0], *outside_points[1]};

            out.vertices = {
                    // The inside point is valid, so keep that...
                    *inside_points[0],
                    // but the two new points are at the locations where the
                    // original sides of the triangle (lines) intersect with the plane
                    line0.intersectPlane(plane),
                    line1.intersectPlane(plane)
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

            Line line0 = {*inside_points[0], *outside_points[0]};

            // The first triangle consists of the two inside points and a new
            // point determined by the location where one side of the triangle
            // intersects with the plane
            out0.vertices = {
                    *inside_points[0],
                    *inside_points[1],
                    line0.intersectPlane(plane)
            };

            Line line1 = {*inside_points[1], *outside_points[0]};

            // The second triangle is composed of one of he inside points, a
            // new point determined by the intersection of the other side of the
            // triangle and the plane, and the newly created point above
            out1.vertices = {
                    *inside_points[1],
                    out0.vertices[2],
                    line1.intersectPlane(plane)
            };

            return {out0, out1}; // Return two newly formed triangles which form a quad
        }

        return {};
    }
};


#endif //V_3D_MYPOLYGON_H
