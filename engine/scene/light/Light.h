//
// Created by iXasthur on 10.10.2021.
//

#ifndef TAMARA_3D_LIGHT_H
#define TAMARA_3D_LIGHT_H

#include "AmbientLight.h"
#include "DiffuseLight.h"
#include "SpecularLight.h"

class Light {
public:
    AmbientLight ambient;
    DiffuseLight diffuse;
    SpecularLight specular;

    Light(
            const AmbientLight &ambient,
            const DiffuseLight &diffuse,
            const SpecularLight &specular
    ) : ambient(ambient), diffuse(diffuse), specular(specular) {

    }
};


#endif //TAMARA_3D_LIGHT_H
