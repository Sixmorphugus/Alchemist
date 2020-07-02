// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"

class Alchemist
{
public:
	/** Alchemist's constructor initializes an instance of the environment. */
	Alchemist();

	/** Alchemist's destructor shuts down an instance of the environment. */
	~Alchemist();

	/** Run() will run the environment until the user closes it. */
	void Run();

private:
	static void _Loop();
	
private:
	bool Close = false;
	
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	SDL_Surface* WindowSurface;
	SDL_Surface* LogoSurface;
	SDL_Texture* LogoTexture;
};