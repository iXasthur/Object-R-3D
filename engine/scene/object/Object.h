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

    std::vector<float> getVertices() {
        std::vector<float> vertices;
        for (Polygon polygon : polygons) {
            for (Vector3 vertex : polygon.vertices) {
                for (float v : vertex.toArray()) {
                    vertices.emplace_back(v);
                }
            }
        }
        return vertices;
    }

//    std::vector<float> getNormals() {
//        std::vector<float> normals;
//        for (Polygon polygon : polygons) {
//            for (Vector3 normal : polygon.normals) {
//                for (float n : normal.toArray()) {
//                    normals.emplace_back(n);
//                }
//            }
//        }
//        return normals;
//    }

    void normalizePolygonVertices() {
        for (Polygon &polygon : polygons) {
            for (Vector3 &vertex : polygon.vertices) {
                vertex.normalize();
            }
        }
    }

    void resizeToHeight(float h) {
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
        Vector3 lowestVertex;

        for (Polygon &polygon : polygons) {
            for (Vector3 &vertex : polygon.vertices) {
                if (vertex.y < lowestVertex.y) {
                    lowestVertex = vertex;
                }
            }
        }

        for (Polygon &polygon : polygons) {
            for (Vector3 &vertex : polygon.vertices) {
                vertex.y -= lowestVertex.y;
            }
        }
    }

    Vector3 dimension() {
        float minX = 0;
        float maxX = 0;
        float minY = 0;
        float maxY = 0;
        float minZ = 0;
        float maxZ = 0;

        if (!polygons.empty()) {
            Vector3 fv = polygons.front().vertices.front();
            minX = fv.x;
            maxX = fv.x;
            minY = fv.y;
            maxY = fv.y;
            minZ = fv.z;
            maxZ = fv.z;

            for (Polygon polygon : polygons) {
                for (Vector3 vertex : polygon.vertices) {
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
        }

        return {maxX - minX, maxY - minY, maxZ - minZ};
    }

//    static Object createCube() {
//        static int cube_c = 0;
//
//        Object cube = Object();
//        cube.name = "cube-" + std::to_string(cube_c++);
//        cube.position = Vector3();
//        cube.polygons = {
//                // SOUTH
//                {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
//                {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
//                // EAST
//                {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
//                {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
//                // NORTH
//                {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
//                {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
//                // WEST
//                {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
//                {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
//                // TOP
//                {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
//                {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
//                // BOTTOM
//                {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
//                {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
//        };
//
//        return cube;
//    }
};


#endif //V_3D_OBJECT_H
