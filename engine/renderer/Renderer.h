//
// Created by iXasthur on 15.09.2021.
//

#ifndef TAMARA_3D_RENDERER_H
#define TAMARA_3D_RENDERER_H

#include <SDL2/SDL.h>
#include <cmath>
#include "../utils/Color.h"

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

    void drawLine(const Vector3 &v0, const Vector3 &v1, const Vector3 &n0, const Vector3 &n1, const Light &light, const Color &color) {
        SDL_Point p0 = {(int) v0.x, (int) v0.y};
        SDL_Point p1 = {(int) v1.x, (int) v1.y};

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
            float zf = Vector3::getLineZtX(v0, v1, (float) x0);
            Vector3 n = Vector3::div(Vector3::add(n0, n1), 2);
            Color c = light.getPixelColor(color, n);

            drawPoint(x, y, zf, c);

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

    void drawTriangle(const Polygon &screenPolygon, const Light &light, const Color &color) {
//        drawLine(screenPolygon, worldPolygon, screenPolygon.vertices[0], screenPolygon.vertices[1], light, color);
//        drawLine(screenPolygon, worldPolygon, screenPolygon.vertices[1], screenPolygon.vertices[2], light, color);
//        drawLine(screenPolygon, worldPolygon, screenPolygon.vertices[2], screenPolygon.vertices[0], light, color);

        if (screenPolygon.isBottomFlat()) {
            drawBottomFlatTriangle(screenPolygon, light, color);
        } else if (screenPolygon.isTopFlat()) {
            drawTopFlatTriangle(screenPolygon, light, color);
        } else {
            auto split = screenPolygon.splitHorizontally();
            drawTopFlatTriangle(split[0], light, color);
            drawBottomFlatTriangle(split[1], light, color);
        }
    }

    void drawTopFlatTriangle(const Polygon &screenPolygon, const Light &light, const Color &color) {
        Polygon drawPolygon = screenPolygon;

        int rc = 0;
        while (std::floor(drawPolygon.vertices[0].y) != std::floor(drawPolygon.vertices[1].y)) {
            if (rc > 2) {
                return;
            }
            drawPolygon = drawPolygon.getRotatedClockwise();
            rc++;
        }

        /*
          0 ---------- 1
            \	     /
             \      /
              \	   /
               \  /
                \/
                 2
        */

        std::array<Vector3, 3> v = drawPolygon.vertices;
        std::array<Vector3, 3> n = drawPolygon.normals;

        for (int i = 0; i < 3; i++) {
            v[i].x = std::floor(v[i].x);
            v[i].y = std::floor(v[i].y);
        }

        float dx0 = (v[0].x - v[2].x) / (v[2].y - v[0].y);
        float dx1 = (v[1].x - v[2].x) / (v[2].y - v[1].y);

        float dz0 = (v[0].z - v[2].z) / (v[2].y - v[0].y);
        float dz1 = (v[1].z - v[2].z) / (v[2].y - v[1].y);

        float xOffset0 = v[2].x;
        float xOffset1 = v[2].x;

        float zOffset0 = v[2].z;
        float zOffset1 = v[2].z;

        for (int scanlineY = (int) v[2].y; scanlineY >= (int) v[0].y; scanlineY--) {
            Vector3 v0 = {xOffset0, (float) scanlineY, zOffset0};
            Vector3 v1 = {xOffset1, (float) scanlineY, zOffset1};
            Vector3 n0 = Vector3::getInterpolatedNormalY(v[0], v[2], n[0], n[2], v0.y);
            Vector3 n1 = Vector3::getInterpolatedNormalY(v[1], v[2], n[1], n[2], v1.y);

            drawLine(v0, v1, n0, n1, light, color);

            xOffset0 += dx0;
            xOffset1 += dx1;
            zOffset0 += dz0;
            zOffset0 += dz1;
        }
    }

    void drawBottomFlatTriangle(const Polygon &screenPolygon, const Light &light, const Color &color) {
        Polygon drawPolygon = screenPolygon;

        int rc = 0;
        while (std::floor(drawPolygon.vertices[1].y) != std::floor(drawPolygon.vertices[2].y)) {
            if (rc > 2) {
                return;
            }
            drawPolygon = drawPolygon.getRotatedClockwise();
            rc++;
        }

        /*
                0
                /\
               /  \
              /    \
             /      \
            /        \
          2 ---------- 1
        */

        std::array<Vector3, 3> v = drawPolygon.vertices;
        std::array<Vector3, 3> n = drawPolygon.normals;

        for (int i = 0; i < 3; i++) {
            v[i].x = std::floor(v[i].x);
            v[i].y = std::floor(v[i].y);
        }

        float dx0 = (v[1].x - v[0].x) / (v[1].y - v[0].y);
        float dx1 = (v[2].x - v[0].x) / (v[2].y - v[0].y);

        float dz0 = (v[1].z - v[0].z) / (v[1].y - v[0].y);
        float dz1 = (v[2].z - v[0].z) / (v[2].y - v[0].y);

        float xOffset0 = v[0].x;
        float xOffset1 = v[0].x;

        float zOffset0 = v[0].z;
        float zOffset1 = v[0].z;

        for (int scanlineY = (int) v[0].y; scanlineY <= (int) v[1].y; scanlineY++) {
            Vector3 v0 = {xOffset0, (float) scanlineY, zOffset0};
            Vector3 v1 = {xOffset1, (float) scanlineY, zOffset1};
            Vector3 n0 = Vector3::getInterpolatedNormalY(v[1], v[0], n[1], n[0], v0.y);
            Vector3 n1 = Vector3::getInterpolatedNormalY(v[2], v[0], n[2], n[0], v1.y);

            drawLine(v0, v1, n0, n1, light, color);

            xOffset0 += dx0;
            xOffset1 += dx1;
            zOffset0 += dz0;
            zOffset0 += dz1;
        }
    }

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
