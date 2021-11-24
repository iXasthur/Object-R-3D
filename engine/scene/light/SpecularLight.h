//
// Created by iXasthur on 10.10.2021.
//

#ifndef OBJECT_R_3D_SPECULARLIGHT_H
#define OBJECT_R_3D_SPECULARLIGHT_H


class SpecularLight {
public:
    float ratio;
    Color color;

    SpecularLight(float ratio, const Color &color) : ratio(ratio), color(color) {

    }

    [[nodiscard]] Color getPixelColor(const Vector3 &objNormal, const float shininess, const Vector3 &lookDirection, const Vector3 &lightDirection) const {
        Vector3 n = Vector3::normalize(objNormal);
        Vector3 v = Vector3::normalize(lookDirection);
        Vector3 l = Vector3::normalize(Vector3::mul(lightDirection, -1));
        float nl2 = 2 * Vector3::dotProduct(n, l);
        Vector3 r = Vector3::sub(l, Vector3::mul(n, nl2));
        float rv = Vector3::dotProduct(r, v);
        if (rv >= 0) {
            float p = powf(rv, shininess);
            return color.exposedRGB(ratio * p);
        } else {
            return {0, 0, 0, 255};
        }
    }
};


#endif //OBJECT_R_3D_SPECULARLIGHT_H
