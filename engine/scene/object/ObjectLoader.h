//
// Created by iXasthur on 16.11.2020.
//

#ifndef V_3D_OBJECTLOADER_H
#define V_3D_OBJECTLOADER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include "Object.h"
#include "SDL2/SDL_image.h"

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

    static Uint32 getPixel(SDL_Surface *surface, int x, int y) {
        int bpp = surface->format->BytesPerPixel;
        /* Here p is the address to the pixel we want to retrieve */
        Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

        switch (bpp) {
            case 1:
                return *p;
                break;
            case 2:
                return *(Uint16 *) p;
                break;
            case 3:
            #if SDL_BYTEORDER == SDL_BIG_ENDIAN
                return p[0] << 16 | p[1] << 8 | p[2];
            #else
                return p[0] | p[1] << 8 | p[2] << 16;
            #endif
                break;
            case 4:
                return *(Uint32 *) p;
                break;
            default:
                throw std::runtime_error("getPixel bpp is not 1, 2, 3 or 4");
        }
    }

    static std::vector<Polygon> loadModel(const std::string &path) {
        std::cout << "Loading " << path << std::endl;

        std::vector<Polygon> polygons;

        std::vector<Vector3> positions;
        std::vector<Vector3> textures;
        std::vector<Vector3> normals;

        std::ifstream in(path, std::ios::in);
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
            throw std::runtime_error("Cannot open " + path);
        }

        return polygons;
    }

    static Texture loadTexture(const std::string &path) {
        std::cout << "Loading " << path << std::endl;

        SDL_Surface *image = IMG_Load(path.c_str());

        if(image == nullptr) {
            printf("IMG_Load: %s\n", IMG_GetError());
            return {};
        }

        std::vector<std::vector<Color>> img = {};

        for(int i = 0; i < image->w - 1; i++) {
            img.emplace_back();
            for (int j = 0; j < image->h - 1; j++) {
                SDL_Color rgb;
                SDL_GetRGB(getPixel(image, i, j), image->format, &rgb.r, &rgb.g, &rgb.b);
                img[i].emplace_back(Color(rgb.r, rgb.g, rgb.b, 255));
            }
        }

        SDL_FreeSurface(image);

        Texture texture;
        texture.img = img;
        return texture;
    }

public:
    static Object loadObject(const std::string &dirpath) {
        std::string name = dirpath;
        unsigned long long find = name.find_last_of('/');
        if (find != std::string::npos) {
            name = name.substr(find + 1);
        }

        find = name.find_last_of('\\');
        if (find != std::string::npos) {
            name = name.substr(find + 1);
        }

        Object obj;
        obj.name = name;
        obj.polygons = ObjectLoader::loadModel(dirpath + "/model.obj");
        obj.albedoMap = ObjectLoader::loadTexture(dirpath + "/albedo_map.png");
        obj.normalMap = ObjectLoader::loadTexture(dirpath + "/normal_map.png");
        obj.specularMap = ObjectLoader::loadTexture(dirpath + "/specular_map.png");

        // Generate maps if empty ?

        return obj;
    }
};


#endif //V_3D_OBJECTLOADER_H
