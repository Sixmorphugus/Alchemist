// Copyright Chris Sixsmith 2020.

#include "DrawShapes.h"

void DrawCircle(SDL_Renderer* Renderer, const Point& Position, int Radius)
{
    const int32_t Diameter = (Radius * 2);

    int32_t x = (Radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - Diameter);

    while (x >= y)
    {
        // Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(Renderer, Position.X + x, Position.Y - y);
        SDL_RenderDrawPoint(Renderer, Position.X + x, Position.Y + y);
        SDL_RenderDrawPoint(Renderer, Position.X - x, Position.Y - y);
        SDL_RenderDrawPoint(Renderer, Position.X - x, Position.Y + y);
        SDL_RenderDrawPoint(Renderer, Position.X + y, Position.Y - x);
        SDL_RenderDrawPoint(Renderer, Position.X + y, Position.Y + x);
        SDL_RenderDrawPoint(Renderer, Position.X - y, Position.Y - x);
        SDL_RenderDrawPoint(Renderer, Position.X - y, Position.Y + x);

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - Diameter);
        }
    }
}
