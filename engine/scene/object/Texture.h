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

    [[nodiscard]] Color getPixelF(float xf, float yf) const {
        int w = (int) img[0].size();
        int h = (int) img.size();

        xf *= (float) (w - 1);
        yf *= (float) (h - 1);
        
        int x = (int) std::round(xf);
        int y = h - (int) std::round(yf);

        return img[x][y];
    }

    static Texture generate(int size, Color color) {
        std::vector<std::vector<Color>> img = {};

        for(int i = 0; i < size; i++) {
            img.emplace_back();
            for (int j = 0; j < size - 1; j++) {
                img[i].emplace_back(color);
            }
        }

        Texture texture;
        texture.img = img;
        return texture;
    }
};


#endif //TAMARA_3D_TEXTURE_H
