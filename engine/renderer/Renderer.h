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

    void drawLine(const Line &line, const Light &light, const Color &color) {
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
            Color c = light.getPixelColor(color, n);

            if (std::isnan(zf)) {
//                zf = std::numeric_limits<float>::lowest();
//                c = {255, 0, 0, 255};
//                drawPoint(x, y, zf, c);
            } else if (std::isnan(n.x) || std::isnan(n.y) || std::isnan(n.z)) {
//                zf = std::numeric_limits<float>::lowest();
//                c = {0, 255, 0, 255};
//                drawPoint(x, y, zf, c);
            } else {
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

    void drawPhongTriangle(const Polygon &screenPolygon, const Light &light, const Color &color) {
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
                drawLine(line, light, color);
            } else {
                float x12 = l12_2d.getXtY(scanlineY);
                float z12 = l12_3d.getZtXY(x12, scanlineY);
                Vector3 n12 = l12_3d.getInterpolatedNormal(x12, scanlineY, z12);
                Vertex v12 = {{x12, scanlineY, z12}, n12};
                Line line = {v12, v02};
                drawLine(line, light, color);
            }

            scanlineY = scanlineY + 1;
        }
    }

    void drawWireframeTriangle(const Polygon &screenPolygon, const Light &light, const Color &color) {
        drawLine({screenPolygon.vertices[0], screenPolygon.vertices[1]}, light, color);
        drawLine({screenPolygon.vertices[1], screenPolygon.vertices[2]}, light, color);
        drawLine({screenPolygon.vertices[2], screenPolygon.vertices[0]}, light, color);
    }

    void drawTriangle(const Polygon &screenPolygon, const Light &light, const Color &color) {
        drawPhongTriangle(screenPolygon, light, color);
//        drawWireframeTriangle(screenPolygon, light, color);
    }



//    void drawTopFlatTriangle(const Polygon &screenPolygon, const Light &light, const Color &color) {
//        Polygon drawPolygon = screenPolygon;
//
//        int rc = 0;
//        while (std::floor(drawPolygon.vertices[0].position.y) != std::floor(drawPolygon.vertices[1].position.y)) {
//            if (rc > 2) {
//                return;
//            }
//            drawPolygon = drawPolygon.getRotatedClockwise();
//            rc++;
//        }
//
//        /*
//          0 ---------- 1
//            \	     /
//             \      /
//              \	   /
//               \  /
//                \/
//                 2
//        */
//
//        std::array<Vertex, 3> v = drawPolygon.vertices;
//        for (int i = 0; i < 3; i++) {
//            v[i].position.x = std::floor(v[i].position.x);
//            v[i].position.y = std::floor(v[i].position.y);
//        }
//
//        Line l02 = {v[0], v[2]};
//        Line l12 = {v[1], v[2]};
//
//        float dx0 = (v[0].position.x - v[2].position.x) / (v[2].position.y - v[0].position.y);
//        float dx1 = (v[1].position.x - v[2].position.x) / (v[2].position.y - v[1].position.y);
//
//        float dz0 = (v[0].position.z - v[2].position.z) / (v[2].position.y - v[0].position.y);
//        float dz1 = (v[1].position.z - v[2].position.z) / (v[2].position.y - v[1].position.y);
//
//        float xOffset0 = v[2].position.x;
//        float xOffset1 = v[2].position.x;
//
//        float zOffset0 = v[2].position.z;
//        float zOffset1 = v[2].position.z;
//
//        for (int scanlineY = (int) v[2].position.y; scanlineY >= (int) v[0].position.y; scanlineY--) {
//            Vector3 v0 = {xOffset0, (float) scanlineY, zOffset0};
//            Vector3 v1 = {xOffset1, (float) scanlineY, zOffset1};
//            Vector3 n0 = l02.getInterpolatedNormalXY(v0.x, v0.y);
//            Vector3 n1 = l12.getInterpolatedNormalXY(v1.x, v1.y);
//
//            Line line = {{v0, n0}, {v1, n1}};
//            drawLine(line, light, color);
//
//            xOffset0 += dx0;
//            xOffset1 += dx1;
//            zOffset0 += dz0;
//            zOffset0 += dz1;
//        }
//    }
//
//    void drawBottomFlatTriangle(const Polygon &screenPolygon, const Light &light, const Color &color) {
//        Polygon drawPolygon = screenPolygon;
//
//        int rc = 0;
//        while (std::floor(drawPolygon.vertices[1].position.y) != std::floor(drawPolygon.vertices[2].position.y)) {
//            if (rc > 2) {
//                return;
//            }
//            drawPolygon = drawPolygon.getRotatedClockwise();
//            rc++;
//        }
//
//        /*
//                0
//                /\
//               /  \
//              /    \
//             /      \
//            /        \
//          2 ---------- 1
//        */
//
//        std::array<Vertex, 3> v = drawPolygon.vertices;
//        for (int i = 0; i < 3; i++) {
//            v[i].position.x = std::floor(v[i].position.x);
//            v[i].position.y = std::floor(v[i].position.y);
//        }
//
//        Line l10 = {v[1], v[0]};
//        Line l20 = {v[2], v[0]};
//
//        float dx0 = (v[1].position.x - v[0].position.x) / (v[1].position.y - v[0].position.y);
//        float dx1 = (v[2].position.x - v[0].position.x) / (v[2].position.y - v[0].position.y);
//
//        float dz0 = (v[1].position.z - v[0].position.z) / (v[1].position.y - v[0].position.y);
//        float dz1 = (v[2].position.z - v[0].position.z) / (v[2].position.y - v[0].position.y);
//
//        float xOffset0 = v[0].position.x;
//        float xOffset1 = v[0].position.x;
//
//        float zOffset0 = v[0].position.z;
//        float zOffset1 = v[0].position.z;
//
//        for (int scanlineY = (int) v[0].position.y; scanlineY <= (int) v[1].position.y; scanlineY++) {
//            Vector3 v0 = {xOffset0, (float) scanlineY, zOffset0};
//            Vector3 v1 = {xOffset1, (float) scanlineY, zOffset1};
//            Vector3 n0 = l10.getInterpolatedNormalXY(v0.x, v0.y);
//            Vector3 n1 = l20.getInterpolatedNormalXY(v1.x, v1.y);
//
//            Line line = {{v0, n0}, {v1, n1}};
//            drawLine(line, light, color);
//
//            xOffset0 += dx0;
//            xOffset1 += dx1;
//            zOffset0 += dz0;
//            zOffset0 += dz1;
//        }
//    }

public:
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

        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
        SDL_RenderClear(renderer);
    }

    void drawPolygon(const Polygon &screenPolygon, const Light &light, const Color &color) {
        if (screenPolygon.isFlat()) {
            return;
        }

        drawTriangle(screenPolygon, light, color);
    }

};


#endif //TAMARA_3D_RENDERER_H
