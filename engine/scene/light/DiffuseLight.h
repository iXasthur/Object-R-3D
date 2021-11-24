//
// Created by iXasthur on 10.10.2021.
//

#ifndef OBJECT_R_3D_DIFFUSELIGHT_H
#define OBJECT_R_3D_DIFFUSELIGHT_H

#include "../../utils/Color.h"

class DiffuseLight {
public:
    float ratio;
    Color color;

    DiffuseLight(float ratio, const Color &color) : ratio(ratio), color(color) {

    }

    [[nodiscard]] Color getPixelColor(const Vector3 &objNormal, const Vector3 &lightDirection) const {
        Vector3 n = Vector3::normalize(objNormal);
        Vector3 l = Vector3::normalize(Vector3::mul(lightDirection, -1));
        float nl = Vector3::dotProduct(n, l);
        if (nl < 0) {
            nl = 0;
        }
        return color.exposedRGB(ratio * nl);
    }
};


#endif //OBJECT_R_3D_DIFFUSELIGHT_H
