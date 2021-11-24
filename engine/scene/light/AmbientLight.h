//
// Created by iXasthur on 10.10.2021.
//

#ifndef OBJECT_R_3D_AMBIENTLIGHT_H
#define OBJECT_R_3D_AMBIENTLIGHT_H

#include "../../utils/Color.h"

class AmbientLight {
public:
    float ratio;
    Color color;

    AmbientLight(float ratio, const Color &color) : ratio(ratio), color(color) {}

    [[nodiscard]] Color getPixelColor() const {
        return color.exposedRGB(ratio);
    }
};


#endif //OBJECT_R_3D_AMBIENTLIGHT_H
