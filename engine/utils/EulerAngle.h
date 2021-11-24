//
// Created by iXasthur on 15.11.2020.
//

#ifndef OBJECT_R_3D_EULERANGLE_H
#define OBJECT_R_3D_EULERANGLE_H


class EulerAngle {
public:
    float x;
    float y;
    float z;

    EulerAngle(float x, float y, float z) : x(x), y(y), z(z) {

    }

    EulerAngle() : x(0), y(0), z(0) {

    }

    [[nodiscard]] std::string toString() const {
        std::string xx = std::to_string(x);
        xx = xx.substr(0, xx.find('.') + 3);

        std::string xy = std::to_string(y);
        xy = xy.substr(0, xx.find('.') + 3);

        std::string xz = std::to_string(z);
        xz = xz.substr(0, xz.find('.') + 3);

        return "x: " + xx + " y: " + xy + " z: " + xz;
    }
};


#endif //OBJECT_R_3D_EULERANGLE_H
