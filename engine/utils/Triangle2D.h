//
// Created by iXasthur on 13.09.2021.
//

#ifndef TAMARA_3D_TRIANGLE2D_H
#define TAMARA_3D_TRIANGLE2D_H

#include "SDL2/SDL.h"

class Triangle2D {
public:
    SDL_Point points[3]{};

    explicit Triangle2D(SDL_Point p0, SDL_Point p1, SDL_Point p2) {
        points[0] = p0;
        points[1] = p1;
        points[2] = p2;
    }
};


#endif //TAMARA_3D_TRIANGLE2D_H
