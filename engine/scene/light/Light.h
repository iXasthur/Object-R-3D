//
// Created by iXasthur on 10.10.2021.
//

#ifndef OBJECT_R_3D_LIGHT_H
#define OBJECT_R_3D_LIGHT_H

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

    [[nodiscard]] Color getPixelColor(const Vector3 &position, const Camera &camera, const Color &ambientColor, const Color &diffuseColor, const Color &specularColor, const Vector3 &normal, const float shininess) const {
        Vector3 lookDirection = Vector3::sub(position, camera.position);

        Color a = ambientColor.exposedRGB(ambient.getPixelColor());
        Color d = diffuseColor.exposedRGB(diffuse.getPixelColor(normal, direction));
        Color s = specularColor.exposedRGB(specular.getPixelColor(normal, shininess, lookDirection, direction));
        return a.plusRGB(d).plusRGB(s);
    }
};


#endif //OBJECT_R_3D_LIGHT_H
