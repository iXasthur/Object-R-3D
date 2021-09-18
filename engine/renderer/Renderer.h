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

    void drawPoint_Z(int x, int y, float zf) {
        SDL_Point point = {x, y};
        if (SDL_PointInRect(&point, &screenRect)) {
            if (zf > 0 && zf < 1 && zf < zBuffer[y][x]) {
                SDL_RenderDrawPoint(renderer, x, y);
                zBuffer[y][x] = zf;
            }
        }
    }

    void drawLine_Z(Vector3 v0, Vector3 v1, Color color) {
        SDL_Point p0 = {(int) v0.x, (int) v0.y};
        SDL_Point p1 = {(int) v1.x, (int) v1.y};

        if (!SDL_IntersectRectAndLine(&screenRect, &p0.x, &p0.y, &p1.x, &p1.y)) {
            return;
        }

        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);

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
            drawPoint_Z(x0, y0, Vector3::getLineZtX(v0, v1, (float) x0));
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


//        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
//
//        float x1 = v0.x;
//        float y1 = v0.y;
//        float z1 = v0.z;
//        float x2 = v1.x;
//        float y2 = v1.y;
//        float z2 = v1.z;
//
//        float i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
//        float di = 1;
//        float point[3];
//
//        point[0] = x1;
//        point[1] = y1;
//        point[2] = z1;
//        dx = x2 - x1;
//        dy = y2 - y1;
//        dz = z2 - z1;
//        x_inc = (dx < 0) ? -1 : 1;
//        l = abs(dx);
//        y_inc = (dy < 0) ? -1 : 1;
//        m = abs(dy);
//        z_inc = (dz < 0) ? -1 : 1;
//        n = abs(dz);
//        dx2 = l * 2;
//        dy2 = m * 2;
//        dz2 = n * 2;
//
//        if ((l >= m) && (l >= n)) {
//            err_1 = dy2 - l;
//            err_2 = dz2 - l;
//
//            i = 0;
//            while (i < l) {
//                drawPoint_Z(point[0], point[1], point[2]);
//                if (err_1 > 0) {
//                    point[1] += y_inc;
//                    err_1 -= dx2;
//                }
//                if (err_2 > 0) {
//                    point[2] += z_inc;
//                    err_2 -= dx2;
//                }
//                err_1 += dy2;
//                err_2 += dz2;
//                point[0] += x_inc;
//
//                i = i + di;
//            }
//        } else if ((m >= l) && (m >= n)) {
//            err_1 = dx2 - m;
//            err_2 = dz2 - m;
//
//            i = 0;
//            while (i < m) {
//                drawPoint_Z(point[0], point[1], point[2]);
//                if (err_1 > 0) {
//                    point[0] += x_inc;
//                    err_1 -= dy2;
//                }
//                if (err_2 > 0) {
//                    point[2] += z_inc;
//                    err_2 -= dy2;
//                }
//                err_1 += dx2;
//                err_2 += dz2;
//                point[1] += y_inc;
//
//                i = i + di;
//            }
//        } else {
//            err_1 = dy2 - n;
//            err_2 = dx2 - n;
//
//            i = 0;
//            while (i < n) {
//                drawPoint_Z(point[0], point[1], point[2]);
//                if (err_1 > 0) {
//                    point[1] += y_inc;
//                    err_1 -= dz2;
//                }
//                if (err_2 > 0) {
//                    point[0] += x_inc;
//                    err_2 -= dz2;
//                }
//                err_1 += dy2;
//                err_2 += dx2;
//                point[2] += z_inc;
//
//                i = i + di;
//            }
//        }
//        drawPoint_Z(point[0], point[1], point[2]);
    }

    void drawTopFlatTriangle_Z(std::array<Vector3, 3> v, Color color) {
        /*
          0 ---------- 1
            \	     /
             \      /
              \	   /
               \  /
                \/
                 2
        */

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
            drawLine_Z(
                    {xOffset0, (float) scanlineY, zOffset0},
                    {xOffset1, (float) scanlineY, zOffset1},
                    color
            );
            xOffset0 += dx0;
            xOffset1 += dx1;
            zOffset0 += dz0;
            zOffset0 += dz1;
        }
    }

    void drawBottomFlatTriangle_Z(std::array<Vector3, 3> v, Color color) {
        /*
                0
                /\
               /  \
              /    \
             /      \
            /        \
          1 ---------- 2
        */

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
            drawLine_Z(
                    {xOffset0, (float) scanlineY, zOffset0},
                    {xOffset1, (float) scanlineY, zOffset1},
                    color
            );
            xOffset0 += dx0;
            xOffset1 += dx1;
            zOffset0 += dz0;
            zOffset0 += dz1;
        }
    }

    void drawLine(SDL_Point p0, SDL_Point p1, Color color) const {
        if (!SDL_IntersectRectAndLine(&screenRect, &p0.x, &p0.y, &p1.x, &p1.y)) {
            return;
        }

        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);

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
            SDL_RenderDrawPoint(renderer, x0, y0);
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
public:
    explicit Renderer(SDL_Renderer *r) : renderer(r) {

    }

    SDL_Rect getScreenRect() {
        return screenRect;
    }

    [[nodiscard]] float getAspectRatio() const {
        return (float) screenRect.h / (float) screenRect.w;
    }

    void updateScreen(Color color) {
        SDL_GetRendererOutputSize(renderer, &screenRect.w, &screenRect.h);
        zBuffer = std::vector<std::vector<float>>(screenRect.h, std::vector<float>(screenRect.w));
        for (int i = 0; i < screenRect.h; ++i) {
            std::fill(zBuffer[i].begin(), zBuffer[i].end(),2);
        }

        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
        SDL_RenderClear(renderer);
    }

    void drawFilledScreenPolygon_Z(Polygon screenPolygon) {
        if (screenPolygon.isFlat()) {
            return;
        }

        // vertices[0] need to have lowest y among points
        std::sort(screenPolygon.vertices.begin(), screenPolygon.vertices.end(), [](Vector3 &v0, Vector3 &v1) {
            return v0.y < v1.y;
        });

        if (std::floor(screenPolygon.vertices[1].y) == std::floor(screenPolygon.vertices[2].y)) {
            drawBottomFlatTriangle_Z(screenPolygon.vertices, screenPolygon.color);

        } else if (std::floor(screenPolygon.vertices[0].y) == std::floor(screenPolygon.vertices[1].y)) {
            drawTopFlatTriangle_Z(screenPolygon.vertices, screenPolygon.color);

        } else {
            Vector3 splitPoint;
            splitPoint.y = screenPolygon.vertices[1].y;
            splitPoint.x = Vector3::getLineXtY(screenPolygon.vertices[0], screenPolygon.vertices[2], splitPoint.y);
            splitPoint.z = Vector3::getLineZtX(screenPolygon.vertices[0], screenPolygon.vertices[2], splitPoint.x);

            std::array<Vector3, 3> points;

            points[0] = screenPolygon.vertices[0];
            points[1] = screenPolygon.vertices[1];
            points[2] = splitPoint;
            drawBottomFlatTriangle_Z(points, screenPolygon.color);

            points[0] = screenPolygon.vertices[1];
            points[1] = splitPoint;
            points[2] = screenPolygon.vertices[2];
            drawTopFlatTriangle_Z(points, screenPolygon.color);
        }
    }

    void drawTriangle2D(Triangle2D tr, Color color) {
        drawLine(tr.points[0], tr.points[1], color);
        drawLine(tr.points[1], tr.points[2], color);
        drawLine(tr.points[2], tr.points[0], color);
    }

};


#endif //TAMARA_3D_RENDERER_H
