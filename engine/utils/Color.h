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

    float Rf;
    float Gf;
    float Bf;
    float Af;

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        R = r;
        G = g;
        B = b;
        A = a;

        Rf = (float) R / 255.0f;
        Gf = (float) G / 255.0f;
        Bf = (float) B / 255.0f;
        Af = (float) A / 255.0f;
    }
};


#endif //V_3D_COLOR_H
