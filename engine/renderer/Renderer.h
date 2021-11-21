//
// Created by iXasthur on 15.09.2021.
//

#ifndef TAMARA_3D_RENDERER_H
#define TAMARA_3D_RENDERER_H

#include <SDL.h>
#include <cmath>
#include <map>
#include <algorithm>
#include "../utils/Color.h"
#include "../scene/object/primitives3/Line.h"
#include "../scene/object/primitives2/Pixel.h"

class Renderer {
private:
    SDL_Renderer *renderer = nullptr;
    SDL_Rect screenRect = {0, 0, 0, 0};
    Color backgroundColor = {0, 0, 0, 255};

    std::vector<Pixel> processTexturedLine(const Line &line, const Scene &scene, const int objIndex) {
        std::vector<Pixel> pixels;

        Vertex vf0 = line.v0;
        vf0.position.x = std::floor(vf0.position.x);
        vf0.position.y = std::floor(vf0.position.y);

        Vertex vf1 = line.v1;
        vf1.position.x = std::floor(vf1.position.x);
        vf1.position.y = std::floor(vf1.position.y);

        Line vLine = {vf0, vf1};

        SDL_Point p0 = {(int) vLine.v0.position.x, (int) vLine.v0.position.y};
        SDL_Point p1 = {(int) vLine.v1.position.x, (int) vLine.v1.position.y};

        if (!SDL_IntersectRectAndLine(&screenRect, &p0.x, &p0.y, &p1.x, &p1.y)) {
            return pixels;
        }

        int x0 = p0.x;
        int y0 = p0.y;
        int x1 = p1.x;
        int y1 = p1.y;

        int dx = abs(x1 - x0);
        int sx = x0 < x1 ? 1 : -1;

        int dy = -abs(y1 - y0);
        int sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;  /* error value e_xy */
        while (true) {
            int x = x0;
            int y = y0;
            float zf = vLine.getZtXY((float) x, (float) y);

            if (std::isnan(zf)) {
//                zf = std::numeric_limits<float>::lowest();
//                Color c = {255, 0, 0, 255};
//                Pixel pixel = {(int) x, (int) y, zf, c};
//                pixels.emplace_back(pixel);
            } else {
                Vector3 tx = vLine.getInterpolatedTexture((float) x, (float) y, zf);

                if (std::isnan(tx.x) || std::isnan(tx.y) || std::isnan(tx.z)) {
//                    zf = std::numeric_limits<float>::lowest();
//                    Color c = {0, 255, 0, 255};
//                    Pixel pixel = {(int) x, (int) y, zf, c};
//                    pixels.emplace_back(pixel);
                } else {
                    Vector3 normal;
                    if (scene.objects[objIndex].normalMap.isEmpty()) {
                        normal = vLine.getInterpolatedNormal((float) x, (float) y, zf);
                    } else {
                        Color normalTx = scene.objects[objIndex].normalMap.getPixelF(tx.x, tx.y);
                        normal = {(normalTx.R * 2 - 255), (normalTx.G * 2 - 255), (normalTx.B * 2 - 255)}; // Will be normalized in getPixelColor(...)
                    }

                    if (std::isnan(normal.x) || std::isnan(normal.y) || std::isnan(normal.z)) {
//                        zf = std::numeric_limits<float>::lowest();
//                        Color c = {0, 0, 255, 255};
//                        Pixel pixel = {(int) x, (int) y, zf, c};
//                        pixels.emplace_back(pixel);
                    } else {
                        Color ambientColor;
                        Color diffuseColor;
                        Color specularColor;
                        float shininess;
                        float opacity;

                        if (scene.objects[objIndex].albedoMap.isEmpty()) {
                            ambientColor = scene.objects[objIndex].material.ambientColor;
                            diffuseColor = scene.objects[objIndex].material.diffuseColor;
                            specularColor = scene.objects[objIndex].material.specularColor;
                            opacity = scene.objects[objIndex].material.opacity;
                        } else {
                            Color txColor = scene.objects[objIndex].albedoMap.getPixelF(tx.x, tx.y);
                            ambientColor = txColor;
                            diffuseColor = txColor;
                            specularColor = txColor;
                            opacity = 1;
                        }

                        if (scene.objects[objIndex].specularMap.isEmpty()) {
                            shininess = scene.objects[objIndex].material.shininess;
                        } else {
                            Color shininessTx = scene.objects[objIndex].specularMap.getPixelF(tx.x, tx.y);
                            shininess = (float) (shininessTx.R + shininessTx.G + shininessTx.B);
                        }

                        Vector3 converted = {(float) x, (float) y, zf};
                        converted = Matrix4::multiplyVector(converted, matScreen_inverse);
                        converted = Matrix4::multiplyVector(converted, matProj_inverse);
                        converted = Matrix4::multiplyVector(converted, matCameraView_inverse);

                        Color c = scene.light.getPixelColor(converted, scene.camera, ambientColor, diffuseColor, specularColor, normal, shininess);
                        c.A = (int) (opacity * 255.0f);

                        Pixel pixel = {(int) x, (int) y, zf, c};
                        pixels.emplace_back(pixel);
                    }
                }
            }

            if (x0 == x1 && y0 == y1) {
                break;
            }
            int e2 = 2 * err;
            if (e2 >= dy) { /* e_xy+e_x > 0 */
                err += dy;
                x0 += sx;
            }
            if (e2 <= dx) { /* e_xy+e_y < 0 */
                err += dx;
                y0 += sy;
            }
        }

        return pixels;
    }

    std::vector<Pixel> processTexturedTriangle(const Polygon &screenPolygon, const Scene &scene, const int objIndex) {
        std::vector<Pixel> pixels;

        std::array<Vertex, 3> vertices = screenPolygon.vertices;
        std::sort(vertices.begin(), vertices.end(), [&](Vertex &v0, Vertex &v1) {
            return v0.position.y < v1.position.y;
        });

        for (int i = 0; i < 3; i++) {
            vertices[i].position.x = std::floor(vertices[i].position.x);
            vertices[i].position.y = std::floor(vertices[i].position.y);
        }

        Line l01_3d = {vertices[0], vertices[1]};
        Line l02_3d = {vertices[0], vertices[2]};
        Line l12_3d = {vertices[1], vertices[2]};

        Line2D l01_2d = {{vertices[0].position.x, vertices[0].position.y}, {vertices[1].position.x, vertices[1].position.y}};
        Line2D l02_2d = {{vertices[0].position.x, vertices[0].position.y}, {vertices[2].position.x, vertices[2].position.y}};
        Line2D l12_2d = {{vertices[1].position.x, vertices[1].position.y}, {vertices[2].position.x, vertices[2].position.y}};

        float scanlineStart = vertices[0].position.y;
        float splitY = vertices[1].position.y;
        float scanlineEnd = vertices[2].position.y;
        float scanlineY = scanlineStart;

        while (scanlineY <= scanlineEnd) {
            float x02 = l02_2d.getXtY(scanlineY);
            float z02 = l02_3d.getZtXY(x02, scanlineY);
            Vector3 n02;
            if (scene.objects[objIndex].normalMap.isEmpty()) {
                n02 = l02_3d.getInterpolatedNormal(x02, scanlineY, z02);
            } else {
                n02 = Vector3::nan();
            }
            Vector3 t02 = l02_3d.getInterpolatedTexture(x02, scanlineY, z02);
            Vertex v02 = {{x02, scanlineY, z02}, t02, n02};

            if (scanlineY < splitY) {
                float x01 = l01_2d.getXtY(scanlineY);
                float z01 = l01_3d.getZtXY(x01, scanlineY);
                Vector3 n01;
                if (scene.objects[objIndex].normalMap.isEmpty()) {
                    n01 = l01_3d.getInterpolatedNormal(x01, scanlineY, z01);
                } else {
                    n01 = Vector3::nan();
                }
                Vector3 t01 = l01_3d.getInterpolatedTexture(x01, scanlineY, z01);
                Vertex v01 = {{x01, scanlineY, z01}, t01, n01};
                Line line = {v01, v02};

                auto ps = processTexturedLine(line, scene, objIndex);
                pixels.insert(pixels.end(), ps.begin(), ps.end());
            } else {
                float x12 = l12_2d.getXtY(scanlineY);
                float z12 = l12_3d.getZtXY(x12, scanlineY);
                Vector3 n12;
                if (scene.objects[objIndex].normalMap.isEmpty()) {
                    n12 = l12_3d.getInterpolatedNormal(x12, scanlineY, z12);
                } else {
                    n12 = Vector3::nan();
                }
                Vector3 t12 = l12_3d.getInterpolatedTexture(x12, scanlineY, z12);
                Vertex v12 = {{x12, scanlineY, z12}, t12, n12};
                Line line = {v12, v02};

                auto ps = processTexturedLine(line, scene, objIndex);
                pixels.insert(pixels.end(), ps.begin(), ps.end());
            }

            scanlineY = scanlineY + 1;
        }

        return pixels;
    }

    std::vector<Pixel> processTriangle(const Polygon &screenPolygon, const Scene &scene, const int objIndex) {
        return processTexturedTriangle(screenPolygon, scene, objIndex);
    }

public:
    Matrix4 matCameraView;
    Matrix4 matProj;
    Matrix4 matScreen;

    Matrix4 matCameraView_inverse;
    Matrix4 matProj_inverse;
    Matrix4 matScreen_inverse;

    explicit Renderer(SDL_Renderer *r) : renderer(r) {
        SDL_GetRendererOutputSize(renderer, &screenRect.w, &screenRect.h);
    }

    [[nodiscard]] SDL_Rect getScreenRect() const {
        return screenRect;
    }

    [[nodiscard]] float getAspectRatio() const {
        return (float) screenRect.h / (float) screenRect.w;
    }

    void updateScreen(const Color &color, const Matrix4 &matCameraView, const Matrix4 &matProj, const Matrix4 &matScreen) {
        SDL_GetRendererOutputSize(renderer, &screenRect.w, &screenRect.h);

        backgroundColor = color;

        this->matCameraView = matCameraView;
        this->matProj = matProj;
        this->matScreen = matScreen;

        this->matCameraView_inverse = Matrix4::invert(matCameraView);
        this->matProj_inverse = Matrix4::invert(matProj);
        this->matScreen_inverse = Matrix4::invert(matScreen);

        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
        SDL_RenderClear(renderer);
    }

    std::vector<Pixel> processPolygon(const Polygon &screenPolygon, const Scene &scene, const int objIndex) {
        if (screenPolygon.isFlat()) {
            return {};
        }

        return processTriangle(screenPolygon, scene, objIndex);
    }

    void drawPixels(const std::vector<std::vector<Pixel>> &scenePixels) {
        std::map<std::pair<int, int>, std::map<int, Pixel>> pxMap;

        for (int objIndex = 0; objIndex < scenePixels.size(); objIndex++) {
            for (const auto &objPixel : scenePixels[objIndex]) {
                SDL_Point point = {(int) objPixel.x, (int) objPixel.y};
                float zf = objPixel.z;

                if (SDL_PointInRect(&point, &screenRect)) {
                    std::pair<int, int> pos = {objPixel.x, objPixel.y};
                    if (pxMap.find(pos) == pxMap.end()) {
                        pxMap[pos] = {};
                        pxMap[pos][objIndex] = objPixel;
                    } else {
                        if (pxMap[pos].find(objIndex) == pxMap[pos].end()) {
                            pxMap[pos][objIndex] = objPixel;
                        } else {
                            if (zf < pxMap[pos][objIndex].z) {
                                pxMap[pos][objIndex] = objPixel;
                            }
                        }
                    }
                }
            }
        }

        for (const auto &[key, value] : pxMap) {
            std::vector<Pixel> pixels = {};
            for (const auto &item : value) {
                pixels.emplace_back(item.second);
            }

            std::sort(pixels.begin(), pixels.end(), [&](const Pixel &p0, const Pixel &p1) {
                return p0.z > p1.z;
            });

            Color blended = backgroundColor;

            for (const auto &pixel : pixels) {
                float a0 = (float) pixel.color.A / 255.0f;
                float a1 = (float) blended.A / 255.0f;
                float r0 = (float) pixel.color.R / 255.0f;
                float r1 = (float) blended.R / 255.0f;
                float g0 = (float) pixel.color.G / 255.0f;
                float g1 = (float) blended.G / 255.0f;
                float b0 = (float) pixel.color.B / 255.0f;
                float b1 = (float) blended.B / 255.0f;

                float a01 = ((1 - a0) * a1) + a0;
                float r01 = (((1 - a0) * a1 * r1) + (a0 * r0)) / a01;
                float g01 = (((1 - a0) * a1 * g1) + (a0 * g0)) / a01;
                float b01 = (((1 - a0) * a1 * b1) + (a0 * b0)) / a01;
                blended = {(int) (r01 * 255.0f), (int) (g01 * 255.0f), (int) (b01 * 255.0f), (int) (a01 * 255.0f)};
            }

            SDL_SetRenderDrawColor(renderer, blended.R, blended.G, blended.B, blended.A);
            SDL_RenderDrawPoint(renderer, key.first, key.second);
        }
    }

};


#endif //TAMARA_3D_RENDERER_H
