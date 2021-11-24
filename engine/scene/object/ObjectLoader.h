//
// Created by iXasthur on 16.11.2020.
//

#ifndef OBJECT_R_3D_OBJECTLOADER_H
#define OBJECT_R_3D_OBJECTLOADER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include "Object.h"
#include "SDL_image.h"
#include "Material.h"

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

    static Texture loadTexture(const std::string &dirpath, const std::string &filename) {
        std::string path = dirpath + '/' + filename;

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

    static std::vector<Material> loadMtl(const std::string &dirpath, const std::string &filename) {
        std::string path = dirpath + '/' + filename;

        std::cout << "Loading " << path << std::endl;

        std::vector<Material> materials;

        std::ifstream in(path, std::ios::in);
        if (in) {
            std::string line;
            while (std::getline(in, line)) {
                if (line.substr(0, 7) == "newmtl ") {
                    materials.emplace_back(Material());
                    std::istringstream v(line.substr(7));
                    v >> materials[materials.size() - 1].name;
                } else if (line.substr(0, 3) == "Ka ") {
                    std::istringstream v(line.substr(3));

                    float rf, gf, bf;
                    v >> rf;
                    v >> gf;
                    v >> bf;

                    materials[materials.size() - 1].ambientColor.R = (int) (rf * 255.0f);
                    materials[materials.size() - 1].ambientColor.G = (int) (gf * 255.0f);
                    materials[materials.size() - 1].ambientColor.B = (int) (bf * 255.0f);
                } else if (line.substr(0, 3) == "Kd ") {
                    std::istringstream v(line.substr(3));

                    float rf, gf, bf;
                    v >> rf;
                    v >> gf;
                    v >> bf;

                    materials[materials.size() - 1].diffuseColor.R = (int) (rf * 255.0f);
                    materials[materials.size() - 1].diffuseColor.G = (int) (gf * 255.0f);
                    materials[materials.size() - 1].diffuseColor.B = (int) (bf * 255.0f);
                } else if (line.substr(0, 3) == "Ks ") {
                    std::istringstream v(line.substr(3));

                    float rf, gf, bf;
                    v >> rf;
                    v >> gf;
                    v >> bf;

                    materials[materials.size() - 1].specularColor.R = (int) (rf * 255.0f);
                    materials[materials.size() - 1].specularColor.G = (int) (gf * 255.0f);
                    materials[materials.size() - 1].specularColor.B = (int) (bf * 255.0f);
                } else if (line.substr(0, 3) == "Ns ") {
                    std::istringstream v(line.substr(3));
                    v >> materials[materials.size() - 1].shininess;
                } else if (line.substr(0, 2) == "d ") {
                    std::istringstream v(line.substr(2));
                    v >> materials[materials.size() - 1].opacity;
                } else if (line.substr(0, 6) == "illum ") {
                    std::istringstream v(line.substr(6));
                    v >> materials[materials.size() - 1].illum;
                } else if (line.substr(0, 7) == "map_Kd ") {
                    std::istringstream v(line.substr(7));
                    std::string txFile;
                    v >> txFile;
                    materials[materials.size() - 1].diffuseMap = loadTexture(dirpath, txFile);
                } else if (line.substr(0, 7) == "map_Ka ") {
                    std::istringstream v(line.substr(7));
                    std::string txFile;
                    v >> txFile;
                    materials[materials.size() - 1].ambientMap = loadTexture(dirpath, txFile);
                } else if (line.substr(0, 5) == "norm ") {
                    std::istringstream v(line.substr(5));
                    std::string txFile;
                    v >> txFile;
                    materials[materials.size() - 1].normalMap = loadTexture(dirpath, txFile);
                } else if (line.substr(0, 7) == "map_Ns ") {
                    std::istringstream v(line.substr(7));
                    std::string txFile;
                    v >> txFile;
                    materials[materials.size() - 1].specularMap = loadTexture(dirpath, txFile);
                }
            }
        } else {
            printf("Cannot open %s", path.c_str());
            return {{}};
        }

        return materials;
    }

    static std::vector<Object> loadObj(const std::string &dirpath, const std::string &filename) {
        std::string path = dirpath + '/' + filename;

        std::cout << "Loading " << path << std::endl;

        std::vector<Object> objects;

        std::vector<Material> materials;

        std::vector<Vector3> positions;
        std::vector<Vector3> textures;
        std::vector<Vector3> normals;

        std::ifstream in(path, std::ios::in);
        if (in) {
            std::string line;
            while (std::getline(in, line)) {
                if (line.substr(0, 2) == "o ") {
                    objects.emplace_back(Object());
                    std::istringstream v(line.substr(2));
                    v >> objects[objects.size() - 1].name;
                } else if (line.substr(0, 7) == "mtllib ") {
                    std::istringstream v(line.substr(7));
                    std::string mtlFile;
                    v >> mtlFile;
                    materials = loadMtl(dirpath, mtlFile);
                } else if (line.substr(0, 7) == "usemtl ") {
                    std::istringstream v(line.substr(7));
                    std::string mtlName;
                    v >> mtlName;
                    for (const auto &material : materials) {
                        if (mtlName == material.name) {
                            objects[objects.size() - 1].material = material;
                            break;
                        }
                    }
                } else if (line.substr(0, 2) == "v ") {
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
                        objects[objects.size() - 1].polygons.emplace_back(polygon);
                    }
                }
            }
        } else {
            throw std::runtime_error("Cannot open " + path);
        }

        return objects;
    }

public:
    static std::vector<Object> loadObjFile(const std::string &dirpath) {
        return loadObj(dirpath, "model.obj");
    }
};


#endif //OBJECT_R_3D_OBJECTLOADER_H
