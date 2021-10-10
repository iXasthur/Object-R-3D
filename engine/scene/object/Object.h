//
// Created by iXasthur on 14.11.2020.
//

#ifndef V_3D_OBJECT_H
#define V_3D_OBJECT_H

#include <vector>
#include "../../utils/Polygon.h"
#include "../../utils/Color.h"

class Object {
public:
    Vector3 position;
    std::vector<Polygon> polygons;
    std::string name;

    Color color = Color(255, 255, 255, 255);

    Object() = default;

    void resizeToHeight(float h) {
        if (polygons.empty()) {
            return;
        }

        Vector3 dim = dimension();
        float delta = h / dim.y;

        for (Polygon &polygon : polygons) {
            for (Vector3 &vertex : polygon.vertices) {
                vertex.x *= delta;
                vertex.y *= delta;
                vertex.z *= delta;
            }
        }
    }

    void centerPolygonVertices() {
        if (polygons.empty()) {
            return;
        }

        float lowestX = polygons[0].vertices[0].x;
        float lowestY = polygons[0].vertices[0].y;

        for (const Polygon &polygon : polygons) {
            for (const Vector3 &vertex : polygon.vertices) {
                if (vertex.y < lowestY) {
                    lowestY = vertex.y;
                }

                if (vertex.x < lowestX) {
                    lowestX = vertex.x;
                }
            }
        }

        for (Polygon &polygon : polygons) {
            for (Vector3 &vertex : polygon.vertices) {
                vertex.y -= lowestY;
                vertex.x -= lowestX;
            }
        }
    }

    [[nodiscard]] Vector3 dimension() const {
        if (polygons.empty()) {
            return {0, 0, 0};
        }

        float minX = polygons[0].vertices[0].x;
        float maxX = polygons[0].vertices[0].x;
        float minY = polygons[0].vertices[0].y;
        float maxY = polygons[0].vertices[0].y;
        float minZ = polygons[0].vertices[0].z;
        float maxZ = polygons[0].vertices[0].z;

        for (const Polygon &polygon : polygons) {
            for (const Vector3 &vertex : polygon.vertices) {
                if (vertex.x < minX) {
                    minX = vertex.x;
                } else if (vertex.x > maxX) {
                    maxX = vertex.x;
                }

                if (vertex.y < minY) {
                    minY = vertex.y;
                } else if (vertex.y > maxY) {
                    maxY = vertex.y;
                }

                if (vertex.z < minZ) {
                    minZ = vertex.z;
                } else if (vertex.z > maxZ) {
                    maxZ = vertex.z;
                }
            }
        }

        return {maxX - minX, maxY - minY, maxZ - minZ};
    }

};


#endif //V_3D_OBJECT_H
