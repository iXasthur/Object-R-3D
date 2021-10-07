//
// Created by iXasthur on 12.09.2021.
//

#ifndef TAMARA_3D_MATRIX4_H
#define TAMARA_3D_MATRIX4_H

#include "Vector3.h"
#include "../scene/camera/Camera.h"

class Matrix4 {
public:
    float m[4][4] = { 0 };

    Matrix4() = default;


    // ---- 3D-GE LIB ----

    static Vector3 multiplyVector(Vector3 &i, Matrix4 &m) {
        Vector3 o;
        o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
        o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
        o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];

        float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
        if (w != 0.0f)
        {
            o.x /= w;
            o.y /= w;
            o.z /= w;
        }
        return o;
    }

    static Matrix4 makeIdentity() {
        Matrix4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    static Matrix4 makeRotationX(float fAngleRad) {
        Matrix4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = cosf(fAngleRad);
        matrix.m[1][2] = sinf(fAngleRad);
        matrix.m[2][1] = -sinf(fAngleRad);
        matrix.m[2][2] = cosf(fAngleRad);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    static Matrix4 makeRotationY(float fAngleRad) {
        Matrix4 matrix;
        matrix.m[0][0] = cosf(fAngleRad);
        matrix.m[0][2] = sinf(fAngleRad);
        matrix.m[2][0] = -sinf(fAngleRad);
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = cosf(fAngleRad);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    static Matrix4 makeRotationZ(float fAngleRad) {
        Matrix4 matrix;
        matrix.m[0][0] = cosf(fAngleRad);
        matrix.m[0][1] = sinf(fAngleRad);
        matrix.m[1][0] = -sinf(fAngleRad);
        matrix.m[1][1] = cosf(fAngleRad);
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    static Matrix4 makeMove(float x, float y, float z) {
        Matrix4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        matrix.m[3][0] = x;
        matrix.m[3][1] = y;
        matrix.m[3][2] = z;
        return matrix;
    }

    static Matrix4 makeScale(float x, float y, float z) {
        Matrix4 matrix;
        matrix.m[0][0] = x;
        matrix.m[1][1] = y;
        matrix.m[2][2] = z;
        matrix.m[3][3] = 1;
        return matrix;
    }

    static Matrix4 makeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar) {
        float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
        Matrix4 matrix;
        matrix.m[0][0] = fAspectRatio * fFovRad;
        matrix.m[1][1] = fFovRad;
        matrix.m[2][2] = fFar / (fFar - fNear);
        matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        matrix.m[2][3] = 1.0f;
        matrix.m[3][3] = 0.0f;
        return matrix;
    }

    static Matrix4 makeScreen(int screenWidth, int screenHeight) {
//        // Mirror Y coordinate
//        projected.y = -projectedV0.y;
//
//        // +0 ... +2
//        projected.x += 1.0f;
//        projected.y += 1.0f;
//
//        // Convert +0 ... +2 to screen width and height
//        projected.x *= 0.5f * (float) screenRect.w;
//        projected.y *= 0.5f * (float) screenRect.h;

        float fHalfWidth = (float) screenWidth * 0.5f;
        float fHalfHeight = (float) screenHeight * 0.5f;

        Matrix4 matrix;
        matrix.m[0][0] = fHalfWidth;
        matrix.m[1][1] = -fHalfHeight;
        matrix.m[2][2] = 1;
        matrix.m[3][3] = 1;
        matrix.m[3][0] = -1.0f + fHalfWidth;
        matrix.m[3][1] = -1.0f + fHalfHeight;
        return matrix;
    }

    static Matrix4 multiplyMatrix(Matrix4 &m1, Matrix4 &m2) {
        Matrix4 matrix;
        for (int c = 0; c < 4; c++) {
            for (int r = 0; r < 4; r++) {
                matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
            }
        }
        return matrix;
    }

    static Matrix4 makeCameraView(Camera camera) {
        Vector3 upVector = camera.getInitialUpVector();
        Vector3 targetVector = camera.getInitialTargetVector();

        Matrix4 m1 = Matrix4::makeRotationX(camera.eulerRotation.x);
        Matrix4 m2 = Matrix4::makeRotationY(camera.eulerRotation.y);
        Matrix4 matCameraRot = Matrix4::multiplyMatrix(m1, m2);
        Vector3 lookDirection = Matrix4::multiplyVector(targetVector, matCameraRot);
        targetVector = Vector3::add(camera.position, lookDirection);
        Matrix4 matCamera = Matrix4::pointAt(camera.position, targetVector, upVector);

        // Quick inverse
        Matrix4 matView;
        matView.m[0][0] = matCamera.m[0][0]; matView.m[0][1] = matCamera.m[1][0]; matView.m[0][2] = matCamera.m[2][0]; matView.m[0][3] = 0.0f;
        matView.m[1][0] = matCamera.m[0][1]; matView.m[1][1] = matCamera.m[1][1]; matView.m[1][2] = matCamera.m[2][1]; matView.m[1][3] = 0.0f;
        matView.m[2][0] = matCamera.m[0][2]; matView.m[2][1] = matCamera.m[1][2]; matView.m[2][2] = matCamera.m[2][2]; matView.m[2][3] = 0.0f;
        matView.m[3][0] = -(matCamera.m[3][0] * matView.m[0][0] + matCamera.m[3][1] * matView.m[1][0] + matCamera.m[3][2] * matView.m[2][0]);
        matView.m[3][1] = -(matCamera.m[3][0] * matView.m[0][1] + matCamera.m[3][1] * matView.m[1][1] + matCamera.m[3][2] * matView.m[2][1]);
        matView.m[3][2] = -(matCamera.m[3][0] * matView.m[0][2] + matCamera.m[3][1] * matView.m[1][2] + matCamera.m[3][2] * matView.m[2][2]);
        matView.m[3][3] = 1.0f;
        return matView;
    }

    static Matrix4 pointAt(Vector3 &pos, Vector3 &target, Vector3 &up) {
        // Calculate new forward direction
        Vector3 newForward = Vector3::sub(target, pos);
        newForward = Vector3::normalize(newForward);

        // Calculate new Up direction
        Vector3 a = Vector3::mul(newForward, Vector3::dotProduct(up, newForward));
        Vector3 newUp = Vector3::sub(up, a);
        newUp = Vector3::normalize(newUp);

        // New Right direction is easy, its just cross product
        Vector3 newRight = Vector3::crossProduct(newUp, newForward);

        // Construct Dimensioning and Translation Matrix
        Matrix4 matrix;
        matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
        return matrix;
    }
};


#endif //TAMARA_3D_MATRIX4_H
