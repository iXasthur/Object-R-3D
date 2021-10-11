//
// Created by iXasthur on 10.10.2021.
//

#ifndef TAMARA_3D_DIFFUSELIGHT_H
#define TAMARA_3D_DIFFUSELIGHT_H

#include "../../utils/Color.h"

class DiffuseLight {
public:
    float ratio;
    Color color;

    DiffuseLight(float ratio, const Color &color) : ratio(ratio), color(color) {

    }

    [[nodiscard]] Color getPixelColor(const Vector3 &objNormal, const Vector3 &objPosition, const Vector3 &lightPosition) const {
        Vector3 n = Vector3::normalize(objNormal);
        Vector3 l = Vector3::normalize(Vector3::sub(objPosition, lightPosition));
        float nl = Vector3::dotProduct(n, l);
        if (nl < 0) {
            nl = 0;
        }
        return color.exposedRGB(ratio * nl);
    }
};


#endif //TAMARA_3D_DIFFUSELIGHT_H
