//
// Created by iXasthur on 10.10.2021.
//

#ifndef TAMARA_3D_LIGHT_H
#define TAMARA_3D_LIGHT_H

#include "AmbientLight.h"
#include "DiffuseLight.h"
#include "SpecularLight.h"
#include "../camera/Camera.h"

class Light {
public:
    Vector3 direction;

    AmbientLight ambient;
    DiffuseLight diffuse;
    SpecularLight specular;

    Light(
            const Vector3 &direction,
            const AmbientLight &ambient,
            const DiffuseLight &diffuse,
            const SpecularLight &specular
    ) : direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {

    }

    [[nodiscard]] Color getPixelColor(const Vector3 &position, const Vector3 &normal, const Camera &camera, const Color &color, const float shininess) const {
        Vector3 lookDirection = Vector3::sub(position, camera.position);

        Color a = ambient.getPixelColor();
        Color d = diffuse.getPixelColor(normal, direction);
        Color s = specular.getPixelColor(normal, shininess, lookDirection, direction);
        return color.exposedRGB(a.plusRGB(d).plusRGB(s));
    }
};


#endif //TAMARA_3D_LIGHT_H
