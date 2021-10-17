//
// Created by iXasthur on 15.09.2021.
//

#ifndef TAMARA_3D_RENDERER_H
#define TAMARA_3D_RENDERER_H

#include <SDL2/SDL.h>
#include <cmath>
#include "../utils/Color.h"
#include "../scene/object/primitives3/Line.h"

class Renderer {
private:
    SDL_Renderer *renderer = nullptr;
    SDL_Rect screenRect = {0, 0, 0, 0};
    std::vector<std::vector<float>> zBuffer = std::vector<std::vector<float>>(0, std::vector<float>(0));

    void drawPoint(int x, int y, float zf, const Color &color) {
        SDL_Point point = {x, y};

        if (SDL_PointInRect(&point, &screenRect)) {
            if (zf < zBuffer[y][x]) {
                SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
                SDL_RenderDrawPoint(renderer, x, y);
                zBuffer[y][x] = zf;
            }
        }
    }

    void drawLine(const Line &line, const Camera &camera, const Light &light, const Color &color, const float shininess) {
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
            return;
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
            Vector3 n = vLine.getInterpolatedNormal((float) x, (float) y, zf);

            if (std::isnan(zf)) {
//                zf = std::numeric_limits<float>::lowest();
//                Color c = {255, 0, 0, 255};
//                drawPoint(x, y, zf, c);
            } else if (std::isnan(n.x) || std::isnan(n.y) || std::isnan(n.z)) {
//                zf = std::numeric_limits<float>::lowest();
//                Color c = {0, 255, 0, 255};
//                drawPoint(x, y, zf, c);
            } else {
                Vector3 converted = {(float) x, (float) y, zf};
                converted = Matrix4::multiplyVector(converted, matScreen_inverse);
                converted = Matrix4::multiplyVector(converted, matProj_inverse);
                converted = Matrix4::multiplyVector(converted, matCameraView_inverse);

                Vector3 lookDirection = Vector3::sub(converted, camera.position);
                Color c = light.getPixelColor(lookDirection, n, color, shininess);
                drawPoint(x, y, zf, c);
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
    }

    void drawPhongTriangle(const Polygon &screenPolygon, const Camera &camera, const Light &light, const Color &color, const float shininess) {
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
            Vector3 n02 = l02_3d.getInterpolatedNormal(x02, scanlineY, z02);
            Vertex v02 = {{x02, scanlineY, z02}, n02};

            if (scanlineY < splitY) {
                float x01 = l01_2d.getXtY(scanlineY);
                float z01 = l01_3d.getZtXY(x01, scanlineY);
                Vector3 n01 = l01_3d.getInterpolatedNormal(x01, scanlineY, z01);
                Vertex v01 = {{x01, scanlineY, z01}, n01};
                Line line = {v01, v02};
                drawLine(line, camera, light, color, shininess);
            } else {
                float x12 = l12_2d.getXtY(scanlineY);
                float z12 = l12_3d.getZtXY(x12, scanlineY);
                Vector3 n12 = l12_3d.getInterpolatedNormal(x12, scanlineY, z12);
                Vertex v12 = {{x12, scanlineY, z12}, n12};
                Line line = {v12, v02};
                drawLine(line, camera, light, color, shininess);
            }

            scanlineY = scanlineY + 1;
        }
    }

    void drawWireframeTriangle(const Polygon &screenPolygon, const Camera &camera, const Light &light, const Color &color, const float shininess) {
        drawLine({screenPolygon.vertices[0], screenPolygon.vertices[1]}, camera, light, color, shininess);
        drawLine({screenPolygon.vertices[1], screenPolygon.vertices[2]}, camera, light, color, shininess);
        drawLine({screenPolygon.vertices[2], screenPolygon.vertices[0]}, camera, light, color, shininess);
    }

    void drawTriangle(const Polygon &screenPolygon, const Camera &camera, const Light &light, const Color &color, const float shininess) {
        drawPhongTriangle(screenPolygon, camera, light, color, shininess);
//        drawWireframeTriangle(screenPolygon, camera, light, color, shininess);
    }

public:
    Matrix4 matCameraView;
    Matrix4 matProj;
    Matrix4 matScreen;

    Matrix4 matCameraView_inverse;
    Matrix4 matProj_inverse;
    Matrix4 matScreen_inverse;

    explicit Renderer(SDL_Renderer *r) : renderer(r) {

    }

    [[nodiscard]] SDL_Rect getScreenRect() const {
        return screenRect;
    }

    [[nodiscard]] float getAspectRatio() const {
        return (float) screenRect.h / (float) screenRect.w;
    }

    void updateScreen(const Color &color) {
        SDL_GetRendererOutputSize(renderer, &screenRect.w, &screenRect.h);
        zBuffer = std::vector<std::vector<float>>(screenRect.h, std::vector<float>(screenRect.w));
        for (int i = 0; i < screenRect.h; ++i) {
            std::fill(zBuffer[i].begin(), zBuffer[i].end(), std::numeric_limits<float>::max());
        }

        matCameraView = {};
        matProj = {};
        matScreen = {};

        matCameraView_inverse = {};
        matProj_inverse = {};
        matScreen_inverse = {};

        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
        SDL_RenderClear(renderer);
    }

    void drawPolygon(const Polygon &screenPolygon, const Camera &camera, const Light &light, const Color &color, const float shininess) {
        if (screenPolygon.isFlat()) {
            return;
        }

        drawTriangle(screenPolygon, camera, light, color, shininess);
    }

};


#endif //TAMARA_3D_RENDERER_H
