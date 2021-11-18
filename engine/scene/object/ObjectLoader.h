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
    static Object loadObjModel(const std::string &filename) {
        auto obj = Object();
        obj.name = filename;

        unsigned long long find = obj.name.find_last_of('/');
        if (find != std::string::npos) {
            obj.name = obj.name.substr(find + 1);
        }

        find = obj.name.find_last_of('\\');
        if (find != std::string::npos) {
            obj.name = obj.name.substr(find + 1);
        }

        std::vector<Polygon> polygons;

        std::vector<Vector3> positions;
        std::vector<Vector3> textures;
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

                    positions.push_back(vertex);
                } else if (line.substr(0, 3) == "vt ") {
                    std::istringstream v(line.substr(3));
                    Vector3 texture;

                    v >> texture.x;
                    v >> texture.y;
                    v >> texture.z;

                    textures.push_back(texture);
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

                    std::vector<Vector3> p;
                    std::vector<Vector3> t;
                    std::vector<Vector3> n;

                    for (auto &faceStr : faceStrs) {
                        std::vector<std::string> faceElementStrs = splitString(faceStr, '/');

                        for (int j = 0; j < faceElementStrs.size(); ++j) {
                            if (!faceElementStrs[j].empty()) {
                                std::istringstream v(faceElementStrs[j]);
                                int fv;
                                v >> fv;

                                switch (j) {
                                    case 0: {
                                        p.emplace_back(positions[fv - 1]);
                                        break;
                                    }
                                    case 1: {
                                        t.emplace_back(textures[fv - 1]);
                                        break;
                                    }
                                    case 2: {
                                        n.emplace_back(normals[fv - 1]);
                                        break;
                                    }
                                    default: {
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    if(p.size() != t.size() && t.size() != n.size()) {
                        throw std::runtime_error("Invalid model opened");
                    }

                    std::vector<Vertex> vertices;
                    for(int i = 0; i < p.size(); i++) {
                        Vertex vertex = {p[i], t[i], n[i]};
                        vertices.emplace_back(vertex);
                    }

                    for(const Polygon &polygon : Polygon::triangulate(vertices)) {
                        polygons.emplace_back(polygon);
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
