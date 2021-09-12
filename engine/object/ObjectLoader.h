//
// Created by iXasthur on 16.11.2020.
//

#ifndef V_3D_OBJECTLOADER_H
#define V_3D_OBJECTLOADER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include "Object.h"

class ObjectLoader {
private:
    static std::vector<std::string> splitString(const std::string &s, char delim) {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;

        while (getline(ss, item, delim)) {
            result.push_back(item);
        }

        return result;
    }

public:
    static Object LoadObjModel(const std::string &filename) {
        Object obj = Object();
        obj.name = filename;

        int find = obj.name.find_last_of('/');
        if (find != std::string::npos) {
            obj.name = obj.name.substr(find + 1);
        }

        find = obj.name.find_last_of('\\');
        if (find != std::string::npos) {
            obj.name = obj.name.substr(find + 1);
        }

        std::vector<Vector3> vertices;
        std::vector<MyPolygon> polygons;
        std::vector<Vector3> normals;

        std::ifstream in(filename, std::ios::in);
        if (in) {
            std::string line;
            while (std::getline(in, line)) {
                if (line.substr(0, 2) == "v ") {
                    std::istringstream v(line.substr(2));
                    Vector3 vertex;

                    v >> vertex.x;
                    v >> vertex.y;
                    v >> vertex.z;

                    vertices.push_back(vertex);
                } else if (line.substr(0, 3) == "vn ") {
                    std::istringstream v(line.substr(3));
                    Vector3 normal;

                    v >> normal.x;
                    v >> normal.y;
                    v >> normal.z;

                    normals.push_back(normal);
                } else if (line.substr(0, 2) == "f ") {
                    std::vector<std::string> faceStrs = splitString(line, ' ');
                    faceStrs.erase(faceStrs.begin());

                    std::vector<int> f;
                    std::vector<int> t;
                    std::vector<int> n;

                    for (auto &faceStr : faceStrs) {
                        std::vector<std::string> faceElementStrs = splitString(faceStr, '/');

                        for (int j = 0; j < faceElementStrs.size(); ++j) {
                            if (!faceElementStrs[j].empty()) {
                                std::istringstream v(faceElementStrs[j]);
                                int fv;
                                v >> fv;

                                switch (j) {
                                    case 0: {
                                        f.emplace_back(fv);
                                        break;
                                    }
                                    case 1: {
                                        t.emplace_back(fv);
                                        break;
                                    }
                                    case 2: {
                                        n.emplace_back(fv);
                                        break;
                                    }
                                    default: {
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    std::array<Vector3, 3> pVertices;
                    std::array<Vector3, 3> pNormals;
                    if (f.size() == 3) {
                        pVertices[0] = vertices[f[0] - 1];
                        pVertices[1] = vertices[f[1] - 1];
                        pVertices[2] = vertices[f[2] - 1];

                        if (!normals.empty()) {
                            pNormals[0] = normals[n[0] - 1];
                            pNormals[1] = normals[n[1] - 1];
                            pNormals[2] = normals[n[2] - 1];
                            polygons.emplace_back(MyPolygon(pVertices, pNormals));
                        } else {
                            polygons.emplace_back(MyPolygon(pVertices));
                        }

                    } else if (f.size() == 4) {
                        pVertices[0] = vertices[f[0] - 1];
                        pVertices[1] = vertices[f[1] - 1];
                        pVertices[2] = vertices[f[2] - 1];

                        if (!normals.empty()) {
                            pNormals[0] = normals[n[0] - 1];
                            pNormals[1] = normals[n[1] - 1];
                            pNormals[2] = normals[n[2] - 1];
                            polygons.emplace_back(MyPolygon(pVertices, pNormals));
                        } else {
                            polygons.emplace_back(MyPolygon(pVertices));
                        }

                        pVertices[0] = vertices[f[0] - 1];
                        pVertices[1] = vertices[f[2] - 1];
                        pVertices[2] = vertices[f[3] - 1];

                        if (!normals.empty()) {
                            pNormals[0] = normals[n[0] - 1];
                            pNormals[1] = normals[n[2] - 1];
                            pNormals[2] = normals[n[3] - 1];
                            polygons.emplace_back(MyPolygon(pVertices, pNormals));
                        } else {
                            polygons.emplace_back(MyPolygon(pVertices));
                        }
                    } else {
                        std::cerr << "Object face must have 3 or 4 vertices" << std::endl;
                    }
                }
            }
        } else {
            std::cerr << "Cannot open " << filename << std::endl;
        }

        obj.polygons = polygons;
        return obj;
    }
};


#endif //V_3D_OBJECTLOADER_H
