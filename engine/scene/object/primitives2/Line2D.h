//
// Created by iXasthur on 12.10.2021.
//

#ifndef TAMARA_3D_LINE2D_H
#define TAMARA_3D_LINE2D_H

#include "Point2D.h"

class Line2D {
public:
    Point2D p0;
    Point2D p1;

    Line2D(const Point2D &p0, const Point2D &p1) : p0(p0), p1(p1) {}

    [[nodiscard]] float getXtY(float targetY) const {
        float x = (targetY - p0.y) * (p1.x - p0.x);
        x /= (p1.y - p0.y);
        x += p0.x;

//        if (std::isnan(x)) {
//            std::cout << "getXtY == nan" << std::endl;
//        }

        return x;
    }

    [[nodiscard]] float getYtX(float targetX) const {
        float y = (targetX - p0.x) * (p1.y - p0.y);
        y /= (p1.x - p0.x);
        y += p0.y;

//        if (std::isnan(y)) {
//            std::cout << "getYtX == nan" << std::endl;
//        }

        return y;
    }

    static Line2D nan() {
        return {Point2D::nan(), Point2D::nan()};
    }
};


#endif //TAMARA_3D_LINE2D_H
