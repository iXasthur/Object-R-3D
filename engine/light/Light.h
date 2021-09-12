//
// Created by iXasthur on 16.11.2020.
//

#ifndef V_3D_LIGHT_H
#define V_3D_LIGHT_H

#include <vector>
#include "../utils/Vector4.h"

class Light {
public:
    Vector4 position;
    std::vector<Vector4> lightPositions;
    int currentLightPositionIndex = -1;

    void nextPosition() {
        if (!lightPositions.empty()) {
            currentLightPositionIndex++;
            if (currentLightPositionIndex > lightPositions.size() - 1) {
                currentLightPositionIndex = 0;
            }

            position = lightPositions[currentLightPositionIndex];
        } else {
            currentLightPositionIndex = -1;
        }
    }
};

#endif //V_3D_LIGHT_H
