//
// Created by iXasthur on 15.11.2020.
//

#ifndef V_3D_COLOR_H
#define V_3D_COLOR_H


class Color {
private:
    [[nodiscard]] Color withFixedBorders() const {
        Color c = *this;
        int *components[4] = {&c.R, &c.G, &c.B, &c.A};
        for (auto &component: components) {
            if (*component > 255) *component = 255;
            if (*component < 0) *component = 0;
        }
        return c;
    }

public:
    int R;
    int G;
    int B;
    int A;

    Color(int r, int g, int b, int a) : R(r), G(g), B(b), A(a) {
//        Rf = (float) R / 255.0f;
//        Gf = (float) G / 255.0f;
//        Bf = (float) B / 255.0f;
//        Af = (float) A / 255.0f;
    }

    [[nodiscard]] Color exposedRGB(float k) const {
        Color c = *this;
        c.R = (int) ((float) c.R * k);
        c.G = (int) ((float) c.G * k);
        c.B = (int) ((float) c.B * k);
        return c.withFixedBorders();
    }

    [[nodiscard]] Color plusRGB(const Color &color) const {
        Color c = *this;
        c.R += color.R;
        c.G += color.G;
        c.B += color.B;
        return c.withFixedBorders();
    }
};


#endif //V_3D_COLOR_H
