//
// Created by iXasthur on 15.11.2020.
//

#ifndef V_3D_COLOR_H
#define V_3D_COLOR_H


class Color {
public:
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : R(r), G(g), B(b), A(a) {
//        Rf = (float) R / 255.0f;
//        Gf = (float) G / 255.0f;
//        Bf = (float) B / 255.0f;
//        Af = (float) A / 255.0f;
    }

    [[nodiscard]] Color exposed(float k) const {
        if (k == 0) {
            return {255, 255, 255, A};
        }

        if (k < 0) {
            return {0, 0, 0, A};
        }

        uint8_t r;
        uint8_t g;
        uint8_t b;

        float fR = (float)R * k;
        float fG = (float)G * k;
        float fB = (float)B * k;

        if (fR > 255.0f) {
            r = 255;
        } else {
            r = (uint8_t)fR;
        }

        if (fG > 255.0f) {
            g = 255;
        } else {
            g = (uint8_t)fG;
        }

        if (fB > 255.0f) {
            b = 255;
        } else {
            b = (uint8_t)fB;
        }

        return {r, g, b, A};
    }
};


#endif //V_3D_COLOR_H
