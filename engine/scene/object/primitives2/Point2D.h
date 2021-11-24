//
// Created by iXasthur on 12.10.2021.
//

#ifndef OBJECT_R_3D_POINT2D_H
#define OBJECT_R_3D_POINT2D_H


class Point2D {
public:
    float x;
    float y;

    Point2D(float x, float y) : x(x), y(y) {}

    Point2D() : x(0), y(0) {}

    static Point2D nan() {
        float nan = std::numeric_limits<float>::quiet_NaN();
        return {nan, nan};
    }
};


#endif //OBJECT_R_3D_POINT2D_H
