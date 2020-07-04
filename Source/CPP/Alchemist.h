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
	
private:
	/** Gets window start size. */
	Size GetWindowStartSize();

	/** Core loop function, declared static for emscripten callback binding. */
	static void _Loop();

#if IS_WEB
	static EM_BOOL _UiEvent(int Type, const EmscriptenUiEvent* UiEvent, void* UserData);
#endif
	
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
