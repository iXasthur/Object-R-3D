//
// Created by iXasthur on 12.09.2021.
//

#ifndef OBJECT_R_3D_MATRIX4_H
#define OBJECT_R_3D_MATRIX4_H

#include "Vector3.h"
#include "EulerAngle.h"

class Matrix4 {
private:
    static float getDeterminant(std::vector<std::vector<float>> vec) {
        if(vec.size() != vec[0].size()) {
            throw std::runtime_error("getDeterminant vec is not quadratic");
        }

        int dimension = (int) vec.size();

        if(dimension == 0) {
            return 1;
        }

        if(dimension == 1) {
            return vec[0][0];
        }

        //Formula for 2x2-matrix
        if(dimension == 2) {
            return vec[0][0] * vec[1][1] - vec[0][1] * vec[1][0];
        }

        float result = 0;
        int sign = 1;
        for(int i = 0; i < dimension; i++) {

            //Submatrix
            std::vector<std::vector<float>> subVect(dimension - 1, std::vector<float> (dimension - 1));
            for(int m = 1; m < dimension; m++) {
                int z = 0;
                for(int n = 0; n < dimension; n++) {
                    if(n != i) {
                        subVect[m-1][z] = vec[m][n];
                        z++;
                    }
                }
            }

            //recursive call
            result = result + (float) sign * vec[0][i] * getDeterminant(subVect);
            sign = -sign;
        }

        return result;
    }

    static std::vector<std::vector<float>> getTranspose(const std::vector<std::vector<float>> &matrix1) {

        //Transpose-matrix: height = width(matrix), width = height(matrix)
        std::vector<std::vector<float>> solution(matrix1[0].size(), std::vector<float> (matrix1.size()));

        //Filling solution-matrix
        for(size_t i = 0; i < matrix1.size(); i++) {
            for(size_t j = 0; j < matrix1[0].size(); j++) {
                solution[j][i] = matrix1[i][j];
            }
        }

        return solution;
    }

    static std::vector<std::vector<float>> getCofactor(const std::vector<std::vector<float>> &vec) {
        if(vec.size() != vec[0].size()) {
            throw std::runtime_error("getDeterminant vec is not quadratic");
        }

        std::vector<std::vector<float>> solution(vec.size(), std::vector<float> (vec.size()));
        std::vector<std::vector<float>> subVec(vec.size() - 1, std::vector<float> (vec.size() - 1));

        for(std::size_t i = 0; i < vec.size(); i++) {
            for(std::size_t j = 0; j < vec[0].size(); j++) {

                int p = 0;
                for(size_t x = 0; x < vec.size(); x++) {
                    if(x == i) {
                        continue;
                    }
                    int q = 0;

                    for(size_t y = 0; y < vec.size(); y++) {
                        if(y == j) {
                            continue;
                        }

                        subVec[p][q] = vec[x][y];
                        q++;
                    }
                    p++;
                }

                solution[i][j] = powf(-1, (float) i + (float) j) * getDeterminant(subVec);
            }
        }

        return solution;
    }

public:
    float m[4][4] = { 0 };

    Matrix4() = default;


    // ---- 3D-GE LIB ----

    static std::vector<std::vector<float>> toVector(const Matrix4 &m) {
        std::vector<std::vector<float>> out = std::vector<std::vector<float>>(4, std::vector<float>(4));
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                out[i][j] = m.m[i][j];
            }
        }
        return out;
    }

    static Matrix4 fromVector(const std::vector<std::vector<float>> &vec) {
        Matrix4 out;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                out.m[i][j] = vec[i][j];
            }
        }
        return out;
    }

    static Vector3 multiplyVector(const Vector3 &i, const Matrix4 &m) {
        Vector3 o;
        o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
        o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
        o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];

        float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
        if (w != 1.0f) {
            if (w == 0.0f) {
//                std::cout << "w == 0" << std::endl;
            }

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
        matrix.m[1][2] = -sinf(fAngleRad);
        matrix.m[2][1] = sinf(fAngleRad);
        matrix.m[2][2] = cosf(fAngleRad);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    static Matrix4 makeRotationY(float fAngleRad) {
        Matrix4 matrix;
        matrix.m[0][0] = cosf(fAngleRad);
        matrix.m[0][2] = sinf(fAngleRad);
        matrix.m[1][1] = 1.0f;
        matrix.m[2][0] = -sinf(fAngleRad);
        matrix.m[2][2] = cosf(fAngleRad);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    static Matrix4 makeRotationZ(float fAngleRad) {
        Matrix4 matrix;
        matrix.m[0][0] = cosf(fAngleRad);
        matrix.m[0][1] = -sinf(fAngleRad);
        matrix.m[1][0] = sinf(fAngleRad);
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
        float fFovRad = fFovDegrees / 180.0f * (float) M_PI;
        float q = 1.0f / tanf(fFovRad / 2.0f);

        Matrix4 matrix;
        matrix.m[0][0] = fAspectRatio * q;
        matrix.m[1][1] = q;
        matrix.m[2][2] = fFar / (fFar - fNear);
        matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        matrix.m[2][3] = 1.0f;
        matrix.m[3][3] = 0.0f;
        return matrix;
    }

    static Matrix4 makeScreen(int screenWidth, int screenHeight) {
        float fHalfWidth = (float) screenWidth * 0.5f;
        float fHalfHeight = (float) screenHeight * 0.5f;

        Matrix4 matrix;
        matrix.m[0][0] = -fHalfWidth;
        matrix.m[1][1] = -fHalfHeight;
        matrix.m[2][2] = 1;
        matrix.m[3][3] = 1;
        matrix.m[3][0] = fHalfWidth;
        matrix.m[3][1] = fHalfHeight;
        return matrix;
    }

    static Matrix4 multiplyMatrix(const Matrix4 &m1, const Matrix4 &m2) {
        Matrix4 matrix;
        for (int c = 0; c < 4; c++) {
            for (int r = 0; r < 4; r++) {
                matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
            }
        }
        return matrix;
    }

    static Matrix4 makeCameraView(const Vector3 &initialUp, const Vector3 &initialTarget, const Vector3 &position, const EulerAngle &rotation) {
        Vector3 upVector = initialUp;
        Vector3 targetVector = initialTarget;

        Matrix4 m1 = Matrix4::makeRotationX(-rotation.x);
        Matrix4 m2 = Matrix4::makeRotationY(-rotation.y);
        Matrix4 matCameraRot = Matrix4::multiplyMatrix(m1, m2);
        Vector3 lookDirection = Matrix4::multiplyVector(targetVector, matCameraRot);
        targetVector = Vector3::add(position, lookDirection);

        Matrix4 matCamera = Matrix4::pointAt(position, targetVector, upVector);

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

    static Matrix4 pointAt(const Vector3 &pos, const Vector3 &target, const Vector3 &up) {
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

    static Matrix4 invert(const Matrix4 &matrix) {
        std::vector<std::vector<float>> vec = Matrix4::toVector(matrix);

        float determinant = getDeterminant(vec);

        if(determinant == 0) {
            throw std::runtime_error("getInverse determinant is 0");
        }

        float d = 1.0f / determinant;

        vec = getTranspose(getCofactor(vec));

        for(size_t i = 0; i < vec.size(); i++) {
            for(size_t j = 0; j < vec.size(); j++) {
                vec[i][j] *= d;
            }
        }

        return Matrix4::fromVector(vec);
    }
};


#endif //OBJECT_R_3D_MATRIX4_H
