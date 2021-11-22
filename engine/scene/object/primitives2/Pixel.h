//
// Created by iXasthur on 22.10.2021.
//

#ifndef TAMARA_3D_PIXEL_H
#define TAMARA_3D_PIXEL_H

#include "../../../utils/Color.h"

class Pixel {
public:
    int x = 0;
    int y = 0;
    float z = 0;
    Color color = {0, 0, 0, 0};

    Pixel() = default;

    Pixel(int x, int y, float z, const Color &color) : x(x), y(y), z(z), color(color) {}
};


#endif //TAMARA_3D_PIXEL_H
