//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_MYPOLYGON_H
#define V_3D_MYPOLYGON_H

#include <array>
#include "Vector3.h"
#include "Color.h"

class Polygon {
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

    Polygon() = default;

    Vector3 getNormal() {
        Vector3 a = vertices[0] - vertices[1];
        Vector3 b = vertices[0] - vertices[2];
        return Vector3::cross(a, b);
    }

    static int clipAgainstPlane(Vector3 plane_p, Vector3 plane_n, Polygon &in_tri, Polygon &out_tri1, Polygon &out_tri2)
    {
        // Make sure plane normal is indeed normal
        plane_n = Vector3::normalize(plane_n);

        // Return signed shortest distance from point to plane, plane normal must be normalised
        auto dist = [&](Vector3 &p)
        {
            //Vector3 n = Vector3_Normalize(p);
            return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector3::dotProduct(plane_n, plane_p));
        };

        // Create two temporary storage arrays to classify points either side of plane
        // If distance sign is positive, point lies on "inside" of plane
        Vector3 *inside_points[3];  int nInsidePointCount = 0;
        Vector3 *outside_points[3]; int nOutsidePointCount = 0;

        // Get signed distance of each point in triangle to plane
        float d0 = dist(in_tri.vertices[0]);
        float d1 = dist(in_tri.vertices[1]);
        float d2 = dist(in_tri.vertices[2]);

        if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.vertices[0]; }
        else { outside_points[nOutsidePointCount++] = &in_tri.vertices[0]; }
        if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.vertices[1]; }
        else { outside_points[nOutsidePointCount++] = &in_tri.vertices[1]; }
        if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.vertices[2]; }
        else { outside_points[nOutsidePointCount++] = &in_tri.vertices[2]; }

        // Now classify triangle points, and break the input triangle into 
        // smaller output triangles if required. There are four possible
        // outcomes...

        if (nInsidePointCount == 0)
        {
            // All points lie on the outside of plane, so clip whole triangle
            // It ceases to exist

            return 0; // No returned triangles are valid
        }

        if (nInsidePointCount == 3)
        {
            // All points lie on the inside of plane, so do nothing
            // and allow the triangle to simply pass through
            out_tri1 = in_tri;

            return 1; // Just the one returned original triangle is valid
        }

        if (nInsidePointCount == 1 && nOutsidePointCount == 2)
        {
            // Triangle should be clipped. As two points lie outside
            // the plane, the triangle simply becomes a smaller triangle

            // Copy appearance info to new triangle
            /*out_tri1.col = in_tri.col;
            out_tri1.sym = in_tri.sym;*/
            /*out_tri1.R = in_tri.R;
            out_tri1.G = in_tri.G;
            out_tri1.B = in_tri.B;*/
            out_tri1.color = in_tri.color;

            // The inside point is valid, so keep that...
            out_tri1.vertices[0] = *inside_points[0];

            // but the two new points are at the locations where the 
            // original sides of the triangle (lines) intersect with the plane
            out_tri1.vertices[1] = Vector3::intersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
            out_tri1.vertices[2] = Vector3::intersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

            return 1; // Return the newly formed single triangle
        }

        if (nInsidePointCount == 2 && nOutsidePointCount == 1)
        {
            // Triangle should be clipped. As two points lie inside the plane,
            // the clipped triangle becomes a "quad". Fortunately, we can
            // represent a quad with two new triangles

            // Copy appearance info to new triangles
            /*out_tri1.col = in_tri.col;
            out_tri1.sym = in_tri.sym;
            out_tri2.col = in_tri.col;
            out_tri2.sym = in_tri.sym;*/
            /*out_tri1.R = in_tri.R;
            out_tri1.G = in_tri.G;
            out_tri1.B = in_tri.B;*/
            out_tri1.color = in_tri.color;

            /*out_tri2.R = in_tri.R;
            out_tri2.G = in_tri.G;
            out_tri2.B = in_tri.B;*/
            out_tri2.color = in_tri.color;

            // The first triangle consists of the two inside points and a new
            // point determined by the location where one side of the triangle
            // intersects with the plane
            out_tri1.vertices[0] = *inside_points[0];
            out_tri1.vertices[1] = *inside_points[1];
            out_tri1.vertices[2] = Vector3::intersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

            // The second triangle is composed of one of he inside points, a
            // new point determined by the intersection of the other side of the 
            // triangle and the plane, and the newly created point above
            out_tri2.vertices[0] = *inside_points[1];
            out_tri2.vertices[1] = out_tri1.vertices[2];
            out_tri2.vertices[2] = Vector3::intersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

            return 2; // Return two newly formed triangles which form a quad
        }

        return -1;
    }
};


#endif //V_3D_MYPOLYGON_H
