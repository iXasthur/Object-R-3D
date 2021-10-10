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
    Vector3 position;

    AmbientLight ambient;
    DiffuseLight diffuse;
    SpecularLight specular;

    Light(
            const Vector3 &position,
            const AmbientLight &ambient,
            const DiffuseLight &diffuse,
            const SpecularLight &specular
    ) : position(position), ambient(ambient), diffuse(diffuse), specular(specular) {

    }

    [[nodiscard]] Color getPixelColor(const Color &objColor, const Vector3 &n) const {
        Color a = ambient.getPixelColor();
//        Color d = diffuse.getPixelColor(n, position);
        Color d = {0, 0, 0, 255};
        Color s = {0, 0, 0, 255};
        return objColor.plusRGB(a).plusRGB(d).plusRGB(s);
    }
};


#endif //TAMARA_3D_LIGHT_H
