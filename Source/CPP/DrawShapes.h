// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"
#include "2DPositioning.h"

// Taken from StackOverflow: https://stackoverflow.com/questions/38334081/howto-draw-circles-arcs-and-vector-graphics-in-sdl#38335842
void DrawCircle(SDL_Renderer* Renderer, const Point& Position, int Radius);
