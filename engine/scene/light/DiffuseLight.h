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

    [[nodiscard]] Color getPixelColor(const Vector3 &n, const Vector3 &pos) const {
        float nl = Vector3::dotProduct(Vector3::normalize(n), pos);
        if (nl < 0) {
            nl = 0;
        }
        return color.exposedRGB(ratio * nl);
    }
};


#endif //TAMARA_3D_DIFFUSELIGHT_H
