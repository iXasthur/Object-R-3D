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

    void drawPoint(int x, int y, float zf) {
        SDL_Point point = {x, y};
        if (SDL_PointInRect(&point, &screenRect)) {
            if (zf < zBuffer[y][x]) {
                SDL_RenderDrawPoint(renderer, x, y);
                zBuffer[y][x] = zf;
            }
        }
    }

    void drawLine(const Vector3 &v0, const Vector3 &v1, const Color &color) {
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
            drawPoint(x0, y0, Vector3::getLineZtX(v0, v1, (float) x0));
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

    void drawTopFlatTriangle(std::array<Vector3, 3> v, const Color &color) {
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
            drawLine(
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

    void drawBottomFlatTriangle(std::array<Vector3, 3> v, const Color &color) {
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
            drawLine(
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

    void drawPolygon(const Polygon &polygon, const Color &color) {
        if (polygon.isFlat()) {
            return;
        }

        std::array<Vector3, 3> vertices = polygon.vertices;
        std::array<Vector3, 3> normals;

        // vertices[0] need to have lowest y among points
        std::sort(vertices.begin(), vertices.end(), [&](const Vector3 &v0, const Vector3 &v1) {
            return v0.y < v1.y;
        });

        for (int i = 0; i < 3; ++i) {
            normals[i] = polygon.getNormalByVertex(vertices[i]);
        }

        Polygon sortedPolygon = Polygon(vertices, normals);

        if (std::floor(sortedPolygon.vertices[1].y) == std::floor(sortedPolygon.vertices[2].y)) {
            drawBottomFlatTriangle(sortedPolygon.vertices, color);

        } else if (std::floor(sortedPolygon.vertices[0].y) == std::floor(sortedPolygon.vertices[1].y)) {
            drawTopFlatTriangle(sortedPolygon.vertices, color);

        } else {
            Vector3 splitPoint;
            splitPoint.y = sortedPolygon.vertices[1].y;
            splitPoint.x = Vector3::getLineXtY(sortedPolygon.vertices[0], sortedPolygon.vertices[2], splitPoint.y);
            splitPoint.z = Vector3::getLineZtX(sortedPolygon.vertices[0], sortedPolygon.vertices[2], splitPoint.x);

            std::array<Vector3, 3> points;

            points[0] = sortedPolygon.vertices[0];
            points[1] = sortedPolygon.vertices[1];
            points[2] = splitPoint;
            drawBottomFlatTriangle(points, color);

            points[0] = sortedPolygon.vertices[1];
            points[1] = splitPoint;
            points[2] = sortedPolygon.vertices[2];
            drawTopFlatTriangle(points, color);
        }
    }

};


#endif //TAMARA_3D_RENDERER_H
