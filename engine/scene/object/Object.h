//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_OBJECT_H
#define V_3D_OBJECT_H

#include <vector>
#include "primitives3/Polygon.h"
#include "../../utils/Color.h"

class Object {
public:
    Vector3 position;
    std::vector<Polygon> polygons;
    std::string name;

    Color color = Color(255, 255, 255, 255);
    float shininess = 20.0f;

    Object() = default;

    void resizeToHeight(float h) {
        if (polygons.empty()) {
            return;
        }

        Vector3 dim = dimension();
        float delta = h / dim.y;

        for (Polygon &polygon : polygons) {
            for (Vertex &vertex : polygon.vertices) {
                vertex.position.x *= delta;
                vertex.position.y *= delta;
                vertex.position.z *= delta;
            }
        }
    }

    void centerPolygonVertices() {
        if (polygons.empty()) {
            return;
        }

        Vector3 dim = dimension();

        float lowestX = polygons[0].vertices[0].position.x;
        float lowestY = polygons[0].vertices[0].position.y;
        float lowestZ = polygons[0].vertices[0].position.z;

        for (const Polygon &polygon : polygons) {
            for (const Vertex &vertex : polygon.vertices) {
                if (vertex.position.y < lowestY) {
                    lowestY = vertex.position.y;
                }

                if (vertex.position.x < lowestX) {
                    lowestX = vertex.position.x;
                }

                if (vertex.position.z < lowestZ) {
                    lowestZ = vertex.position.z;
                }
            }
        }

        for (Polygon &polygon : polygons) {
            for (Vertex &vertex : polygon.vertices) {
                vertex.position.x -= lowestX + dim.x / 2;
                vertex.position.y -= lowestY + dim.y / 2;
                vertex.position.z -= lowestZ + dim.z / 2;
            }
        }
    }

    [[nodiscard]] Vector3 dimension() const {
        if (polygons.empty()) {
            return {0, 0, 0};
        }

        float minX = polygons[0].vertices[0].position.x;
        float maxX = polygons[0].vertices[0].position.x;
        float minY = polygons[0].vertices[0].position.y;
        float maxY = polygons[0].vertices[0].position.y;
        float minZ = polygons[0].vertices[0].position.z;
        float maxZ = polygons[0].vertices[0].position.z;

        for (const Polygon &polygon : polygons) {
            for (const Vertex &vertex : polygon.vertices) {
                if (vertex.position.x < minX) {
                    minX = vertex.position.x;
                } else if (vertex.position.x > maxX) {
                    maxX = vertex.position.x;
                }

                if (vertex.position.y < minY) {
                    minY = vertex.position.y;
                } else if (vertex.position.y > maxY) {
                    maxY = vertex.position.y;
                }

                if (vertex.position.z < minZ) {
                    minZ = vertex.position.z;
                } else if (vertex.position.z > maxZ) {
                    maxZ = vertex.position.z;
                }
            }
        }

        return {maxX - minX, maxY - minY, maxZ - minZ};
    }

};


#endif //V_3D_OBJECT_H
