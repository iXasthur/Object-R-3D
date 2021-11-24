//
// Created by iXasthur on 14.11.2020.
//

#ifndef OBJECT_R_3D_OBJECT_H
#define OBJECT_R_3D_OBJECT_H

#include <vector>
#include "primitives3/Polygon.h"
#include "../../utils/Color.h"
#include "Texture.h"
#include "Material.h"

class Object {
public:
    Vector3 position;
    std::vector<Polygon> polygons;
    std::string name;

    EulerAngle rotation;

    Material material = {};

    Object() = default;

    static void resizeToHeight(std::vector<Object> &objects, float h) {
        if (objects.empty()) {
            return;
        }

        Vector3 dim = dimension(objects);
        float delta = h / dim.y;

        for (Object &object : objects) {
            for (Polygon &polygon: object.polygons) {
                for (Vertex &vertex: polygon.vertices) {
                    vertex.position.x *= delta;
                    vertex.position.y *= delta;
                    vertex.position.z *= delta;
                }
            }
        }
    }

    static void centerVertices(std::vector<Object> &objects) {
        if (objects.empty()) {
            return;
        }

        Vector3 dim = dimension(objects);

        float lowestX = objects[0].polygons[0].vertices[0].position.x;
        float lowestY = objects[0].polygons[0].vertices[0].position.y;
        float lowestZ = objects[0].polygons[0].vertices[0].position.z;

        for (const auto &object : objects) {
            for (const Polygon &polygon: object.polygons) {
                for (const Vertex &vertex: polygon.vertices) {
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
        }

        for (Object &object : objects) {
            for (Polygon &polygon: object.polygons) {
                for (Vertex &vertex: polygon.vertices) {
                    vertex.position.x -= lowestX + dim.x / 2;
                    vertex.position.y -= lowestY + dim.y / 2;
                    vertex.position.z -= lowestZ + dim.z / 2;
                }
            }
        }
    }

    static Vector3 dimension(const std::vector<Object> &objects) {
        if (objects.empty()) {
            return {0, 0, 0};
        }

        float minX = objects[0].polygons[0].vertices[0].position.x;
        float maxX = objects[0].polygons[0].vertices[0].position.x;
        float minY = objects[0].polygons[0].vertices[0].position.y;
        float maxY = objects[0].polygons[0].vertices[0].position.y;
        float minZ = objects[0].polygons[0].vertices[0].position.z;
        float maxZ = objects[0].polygons[0].vertices[0].position.z;

        for (const auto &object : objects) {
            for (const Polygon &polygon : object.polygons) {
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
        }

        return {maxX - minX, maxY - minY, maxZ - minZ};
    }

};


#endif //OBJECT_R_3D_OBJECT_H
