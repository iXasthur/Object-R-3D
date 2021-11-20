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
private:
    [[nodiscard]] Color getPixelF_bilinear(float xf, float yf) const {
        int w = (int) img[0].size();
        int h = (int) img.size();

        yf = 1 - yf;

        xf *= (float) (w - 1);
        yf *= (float) (h - 1);

        int x0 = std::floor(xf);
        int y0 = std::floor(yf);
        int x1 = std::ceil(xf);
        int y1 = std::ceil(yf);

        float x0f = (float) x0;
        float y0f = (float) y0;
        float x1f = (float) x1;
        float y1f = (float) y1;

        Color colors[2][2];
        colors[0][0] = img[x0][y0];
        colors[0][1] = img[x0][y1];
        colors[1][0] = img[x1][y0];
        colors[1][1] = img[x1][y1];

        Vertex colorsV[2][2];
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                Vector3 vector = {colors[i][j].R, colors[i][j].G, colors[i][j].B};
                colorsV[i][j] = {{i, j, 0}, {}, vector};
            }
        }

        Line lineTop = {colorsV[0][0], colorsV[1][0]};
        Line lineBot = {colorsV[0][1], colorsV[1][1]};

        Vector3 topInterpolated = lineTop.getInterpolatedNormal(xf - x0f, lineTop.v0.position.y, 0);
        Vector3 botInterpolated = lineBot.getInterpolatedNormal(xf - x0f, lineBot.v0.position.y, 0);

        Vertex topInterpolatedV = {{xf - x0f, lineTop.v0.position.y, 0.0f}, {}, topInterpolated};
        Vertex botInterpolatedV = {{xf - x0f, lineBot.v0.position.y, 0.0f}, {}, botInterpolated};

        Line verticalLine = {topInterpolatedV, botInterpolatedV};
        Vector3 verticalInterpolated = verticalLine.getInterpolatedNormal(xf - x0f, yf - y0f, 0);

        if (std::isnan(verticalInterpolated.x) || std::isnan(verticalInterpolated.y) || std::isnan(verticalInterpolated.z)) {
            return getPixelF_linear(xf, yf);
        }

        Color color = {(int) std::round(verticalInterpolated.x), (int) std::round(verticalInterpolated.y), (int) std::round(verticalInterpolated.z), 0};
        return color;
    }

    [[nodiscard]] Color getPixelF_linear(float xf, float yf) const {
        int x = (int) std::round(xf * (float) (img[0].size() - 1));
        int y = (int) (img.size() - 1) - (int) std::round(yf * (float) (img.size() - 1));
        return img[x][y];
    }

public:
    std::vector<std::vector<Color>> img = {};

    Texture() = default;

    [[nodiscard]] bool isEmpty() const {
        return img.empty();
    }

    [[nodiscard]] Color getPixelF(float xf, float yf) const {
        return getPixelF_bilinear(xf, yf);
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
