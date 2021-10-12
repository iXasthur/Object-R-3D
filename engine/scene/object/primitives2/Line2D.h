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

    static Point2D intersection(const Line2D &l0, const Line2D &l1) {
        // TODO
        std::cout << "intersection == nan" << std::endl;
        return {};
    }
};


#endif //TAMARA_3D_LINE2D_H
