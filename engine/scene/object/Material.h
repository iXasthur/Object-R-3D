//
// Created by Mikhail on 21.11.2021.
//

#ifndef TAMARA_3D_MATERIAL_H
#define TAMARA_3D_MATERIAL_H

#include <string>
#include "../../utils/Color.h"

class Material {
public:
    std::string name = "unknown_material";
    Color ambientColor = {255, 255, 255 ,255}; // Ka
    Color diffuseColor = {255, 255, 255 ,255}; // Kd
    Color specularColor = {255, 255, 255 ,255}; // Ks
    float shininess = 20; // Ns
    float opacity = 1; // d
    int illum = 2; // illum

    Texture diffuseMap = {};
    Texture ambientMap = {};
    Texture normalMap = {};
    Texture specularMap = {};
};


#endif //TAMARA_3D_MATERIAL_H
