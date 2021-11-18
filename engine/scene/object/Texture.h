//
// Created by Mikhail on 18.11.2021.
//

#ifndef TAMARA_3D_TEXTURE_H
#define TAMARA_3D_TEXTURE_H

#include <utility>
#include <vector>
#include "../../utils/Color.h"
#include "../../utils/Vector3.h"

class Texture {
public:
    std::vector<std::vector<Color>> img = {};

    Texture() = default;

    [[nodiscard]] bool isEmpty() const {
        return img.empty();
    }

    [[nodiscard]] Color getPixel(float xf, float yf) const {
        int x = (int)std::round(xf);
        int y = (int)std::round(yf);
        return img[x][y];
    }
};


#endif //TAMARA_3D_TEXTURE_H
