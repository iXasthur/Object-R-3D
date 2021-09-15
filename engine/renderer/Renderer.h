//
// Created by iXasthur on 15.09.2021.
//

#ifndef TAMARA_3D_RENDERER_H
#define TAMARA_3D_RENDERER_H

#include <SDL2/SDL.h>
#include "../utils/Color.h"

class Renderer {
private:
    SDL_Renderer *renderer = nullptr;
    SDL_Rect screenRect = {0, 0, 0, 0};

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

    void drawTopFlatTriangle(SDL_Point *v, Color color) const {
        /*
          0 ---------- 1
            \	     /
             \      /
              \	   /
               \  /
                \/
                 2
        */
        float dx0 = (float) (v[0].x - v[2].x) / (float) (v[2].y - v[0].y);
        float dx1 = (float) (v[1].x - v[2].x) / (float) (v[2].y - v[1].y);

        float xOffset0 = v[2].x;
        float xOffset1 = v[2].x;

        for (int scanlineY = v[2].y; scanlineY >= v[0].y; scanlineY--) {
            drawLine({(int)xOffset0, scanlineY}, {(int)xOffset1, scanlineY}, color);
            xOffset0 += dx0;
            xOffset1 += dx1;
        }
    }

    void drawBottomFlatTriangle(SDL_Point *v, Color color) {
        /*
                0
                /\
               /  \
              /    \
             /      \
            /        \
          1 ---------- 2
        */
        float dx0 = (float) (v[1].x - v[0].x) / (float) (v[1].y - v[0].y);
        float dx1 = (float) (v[2].x - v[0].x) / (float) (v[2].y - v[0].y);

        float xOffset0 = v[0].x;
        float xOffset1 = v[0].x;

        for (int scanlineY = v[0].y; scanlineY <= v[1].y; scanlineY++) {
            drawLine({(int)xOffset0, scanlineY}, {(int)xOffset1, scanlineY}, color);
            xOffset0 += dx0;
            xOffset1 += dx1;
        }
    }

public:
    explicit Renderer(SDL_Renderer *r) : renderer(r) {

    }

    SDL_Rect getScreenRect() {
        return screenRect;
    }

    float getAspectRatio() {
        return (float) screenRect.h / (float) screenRect.w;
    }

    void updateScreenRect() {
        SDL_GetRendererOutputSize(renderer, &screenRect.w, &screenRect.h);
    }

    void clear(Color color) {
        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
        SDL_RenderClear(renderer);
    }

    void drawTriangle2D(Triangle2D tr, Color color) {
        drawLine(tr.points[0], tr.points[1], color);
        drawLine(tr.points[1], tr.points[2], color);
        drawLine(tr.points[2], tr.points[0], color);
    }

    void drawFilledTriangle2D(Triangle2D tr, Color color) {
        // Points are Integers
        // p[0] need to have lowest y among points
        if (!(tr.points[0].y == tr.points[1].y && tr.points[1].y == tr.points[2].y)) {
            bool sorted = false;
            while (!sorted) {
                sorted = true;
                for (int i = 0; i < 2; i++) {
                    if (tr.points[i].y > tr.points[i + 1].y) {
                        sorted = false;
                        SDL_Point buffPoint = tr.points[i];
                        tr.points[i] = tr.points[i + 1];
                        tr.points[i + 1] = buffPoint;
                    }
                }
            }

            if (tr.points[1].y == tr.points[2].y) {
                drawBottomFlatTriangle(tr.points, color);
            } else if (tr.points[0].y == tr.points[1].y) {
                drawTopFlatTriangle(tr.points, color);
            } else {
                SDL_Point splitPoint;
                splitPoint.x = tr.points[0].x +
                               ((float) (tr.points[1].y - tr.points[0].y) / (float) (tr.points[2].y - tr.points[0].y)) *
                               (tr.points[2].x - tr.points[0].x);
                splitPoint.y = tr.points[1].y;
                SDL_Point points[3];
                points[0] = tr.points[0];
                points[1] = tr.points[1];
                points[2] = splitPoint;
                drawBottomFlatTriangle(points, color);
                points[0] = tr.points[1];
                points[1] = splitPoint;
                points[2] = tr.points[2];
                drawTopFlatTriangle(points, color);
            }
        }

    }
};


#endif //TAMARA_3D_RENDERER_H
