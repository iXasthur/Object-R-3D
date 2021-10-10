//
// Created by iXasthur on 10.10.2021.
//

#ifndef TAMARA_3D_AMBIENTLIGHT_H
#define TAMARA_3D_AMBIENTLIGHT_H

#include "../../utils/Color.h"

class AmbientLight {
public:
    float ratio;
    Color color;

    AmbientLight(float ratio, const Color &color) : ratio(ratio), color(color) {}

    [[nodiscard]] Color getValue() const {
        return color.exposed(ratio);
    }
};


#endif //TAMARA_3D_AMBIENTLIGHT_H
