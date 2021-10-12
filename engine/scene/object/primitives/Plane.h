//
// Created by iXasthur on 12.10.2021.
//

#ifndef TAMARA_3D_PLANE_H
#define TAMARA_3D_PLANE_H

#include "../../../utils/Vector3.h"

class Plane {
public:
    Vector3 p;
    Vector3 n;

    Plane(Vector3 p, Vector3 n) : p(p), n(n) {}
};


#endif //TAMARA_3D_PLANE_H
