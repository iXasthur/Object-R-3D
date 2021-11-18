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

    explicit Color() : R(0), G(0), B(0), A(255) {

    }

    Color(int r, int g, int b, int a) : R(r), G(g), B(b), A(a) {

    }

    [[nodiscard]] Color exposedRGB(Color color) const {
        Color c = *this;
        c.R = (int) ((float) c.R * ((float) color.R) / 255.0f);
        c.G = (int) ((float) c.G * ((float) color.G) / 255.0f);
        c.B = (int) ((float) c.B * ((float) color.B) / 255.0f);
        return c.withFixedBorders();
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

    [[nodiscard]] std::string toString() const {
        std::string xr = std::to_string(R);
        xr = xr.substr(0, xr.find('.') + 3);

        std::string xg = std::to_string(G);
        xg = xg.substr(0, xr.find('.') + 3);

        std::string xb = std::to_string(B);
        xb = xb.substr(0, xb.find('.') + 3);

        std::string xa = std::to_string(A);
        xa = xa.substr(0, xb.find('.') + 3);

        return "r: " + xr + " g: " + xg + " b: " + xb + " a: " + xa;
    }
};


#endif //V_3D_COLOR_H
