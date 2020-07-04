// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"
#include "2DPositioning.h"

class Alchemist
{
public:
	/** Alchemist's constructor initializes an instance of the environment. */
	Alchemist();

	/** Alchemist's destructor shuts down an instance of the environment. */
	~Alchemist();

	/** Run() will run the environment until the user closes it. */
	void Run();

	/** Core loop inner function. Processes a single Frame when called. */
	void Frame();

	/** Handles emscripten ui resize event. */
#if IS_WEB
	EM_BOOL UiEvent(int Type, const EmscriptenUiEvent* UiEvent);
#endif
	
private:
	/** Gets window start size. */
	Size _GetWindowStartSize();
	
private:
	bool Close = false;
	bool ViewDrag = false;

	Point ViewTopLeft;
	
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	SDL_Surface* WindowSurface;

	TTF_Font* Font;
	
	SDL_Surface* TestImageSurface;
	SDL_Texture* TestImageTexture;
};
