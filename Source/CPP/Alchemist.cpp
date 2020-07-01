// Copyright Chris Sixsmith 2020.

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	SDL_Init(SDL_INIT_VIDEO);

	// Somehow, we need to track the size of the camera and keep the internal SDL2 window the same size.

	return 0;
}
