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
    Vector3 position;

    Object *model;

    AmbientLight ambient;
    DiffuseLight diffuse;
    SpecularLight specular;

    Light(
            const Vector3 &position,
            const AmbientLight &ambient,
            const DiffuseLight &diffuse,
            const SpecularLight &specular
    ) : position(position), model(nullptr), ambient(ambient), diffuse(diffuse), specular(specular) {

    }

    [[nodiscard]] Color getPixelColor(const Vector3 &position, const Vector3 &normal, const Camera &camera, const Color &color, const float shininess) const {
        Vector3 lookDirection = Vector3::sub(position, camera.position);
        Vector3 lightDirection = Vector3::sub(position, this->position);

        Color a = ambient.getPixelColor();
        Color d = diffuse.getPixelColor(normal, lightDirection);
        Color s = specular.getPixelColor(normal, shininess, lookDirection, lightDirection);
        return color.plusRGB(a).plusRGB(d).plusRGB(s);
    }

    [[nodiscard]] Object getObject() const {
        if (model == nullptr) {
            throw std::runtime_error("Light has no model");
        }

        Object obj = *model;
        obj.position = position;
        return obj;
    }
};


#endif //TAMARA_3D_LIGHT_H
