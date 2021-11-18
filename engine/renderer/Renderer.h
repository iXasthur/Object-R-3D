//
// Created by iXasthur on 15.09.2021.
//

#ifndef TAMARA_3D_RENDERER_H
#define TAMARA_3D_RENDERER_H

#include <SDL2/SDL.h>
#include <cmath>
#include "../utils/Color.h"
#include "../scene/object/primitives3/Line.h"
#include "../scene/object/primitives2/Pixel.h"

class Renderer {
private:
    SDL_Renderer *renderer = nullptr;
    SDL_Rect screenRect = {0, 0, 0, 0};
    std::vector<std::vector<float>> zBuffer = std::vector<std::vector<float>>(0, std::vector<float>(0));

    std::vector<Pixel> processTexturedLine(const Line &line, const Scene &scene) {
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

            Vector3 tx = vLine.getInterpolatedTexture((float) x, (float) y, zf);

            if (std::isnan(zf) || std::isnan(tx.x) || std::isnan(tx.y) || std::isnan(tx.z)) {
//                zf = std::numeric_limits<float>::lowest();
//                Color c = {0, 255, 0, 255};
//                Pixel pixel = {(int) x, (int) y, zf, c};
//                pixels.emplace_back(pixel);
            } else {
                Color nt = scene.object.normalMap.getPixelF(tx.x, tx.y);
                Color st = scene.object.specularMap.getPixelF(tx.x, tx.y);

                Color txA = scene.object.albedoMap.getPixelF(tx.x, tx.y);
                Vector3 txN = {(nt.R * 2 - 255), (nt.G * 2 - 255), (nt.B * 2 - 255)};
                auto txS = (float) (st.R + st.G + st.B);

                Vector3 converted = {(float) x, (float) y, zf};
                converted = Matrix4::multiplyVector(converted, matScreen_inverse);
                converted = Matrix4::multiplyVector(converted, matProj_inverse);
                converted = Matrix4::multiplyVector(converted, matCameraView_inverse);

                Color c = scene.light.getPixelColor(converted, scene.camera, txA, txN, txS);

                Pixel pixel = {(int) x, (int) y, zf, c};
                pixels.emplace_back(pixel);
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

    std::vector<Pixel> processTexturedTriangle(const Polygon &screenPolygon, const Scene &scene) {
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
            Vector3 n02 = Vector3::nan();
            Vector3 t02 = l02_3d.getInterpolatedTexture(x02, scanlineY, z02);
            Vertex v02 = {{x02, scanlineY, z02}, t02, n02};

            if (scanlineY < splitY) {
                float x01 = l01_2d.getXtY(scanlineY);
                float z01 = l01_3d.getZtXY(x01, scanlineY);
                Vector3 n01 = Vector3::nan();
                Vector3 t01 = l01_3d.getInterpolatedTexture(x01, scanlineY, z01);
                Vertex v01 = {{x01, scanlineY, z01}, t01, n01};
                Line line = {v01, v02};

                auto ps = processTexturedLine(line, scene);
                pixels.insert(pixels.end(), ps.begin(), ps.end());
            } else {
                float x12 = l12_2d.getXtY(scanlineY);
                float z12 = l12_3d.getZtXY(x12, scanlineY);
                Vector3 n12 = Vector3::nan();
                Vector3 t12 = l12_3d.getInterpolatedTexture(x12, scanlineY, z12);
                Vertex v12 = {{x12, scanlineY, z12}, t12, n12};
                Line line = {v12, v02};

                auto ps = processTexturedLine(line, scene);
                pixels.insert(pixels.end(), ps.begin(), ps.end());
            }

            scanlineY = scanlineY + 1;
        }

        return pixels;
    }

    std::vector<Pixel> processTriangle(const Polygon &screenPolygon, const Scene &scene) {
        return processTexturedTriangle(screenPolygon, scene);
    }

public:
    Matrix4 matCameraView;
    Matrix4 matProj;
    Matrix4 matScreen;

    Matrix4 matCameraView_inverse;
    Matrix4 matProj_inverse;
    Matrix4 matScreen_inverse;

    bool forceObjectColor = false;

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
        zBuffer = std::vector<std::vector<float>>(screenRect.h, std::vector<float>(screenRect.w));
        for (int i = 0; i < screenRect.h; ++i) {
            std::fill(zBuffer[i].begin(), zBuffer[i].end(), std::numeric_limits<float>::max());
        }

        this->matCameraView = matCameraView;
        this->matProj = matProj;
        this->matScreen = matScreen;

        this->matCameraView_inverse = Matrix4::invert(matCameraView);
        this->matProj_inverse = Matrix4::invert(matProj);
        this->matScreen_inverse = Matrix4::invert(matScreen);

        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
        SDL_RenderClear(renderer);
    }

    std::vector<Pixel> processPolygon(const Polygon &screenPolygon, const Scene &scene) {
        if (screenPolygon.isFlat()) {
            return {};
        }

        return processTriangle(screenPolygon, scene);
    }

    void drawPixels(const std::vector<Pixel> &pixels) {
        for (const auto &pixel : pixels) {
            SDL_Point point = {(int) pixel.x, (int) pixel.y};
            float zf = pixel.z;

            if (SDL_PointInRect(&point, &screenRect)) {
                if (zf < zBuffer[point.y][point.x]) {
                    SDL_SetRenderDrawColor(renderer, pixel.color.R, pixel.color.G, pixel.color.B, pixel.color.A);
                    SDL_RenderDrawPoint(renderer, pixel.x, pixel.y);
                    zBuffer[point.y][point.x] = zf;
                }
            }
        }
    }

};


#endif //TAMARA_3D_RENDERER_H
