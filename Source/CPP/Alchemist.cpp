// Copyright Chris Sixsmith 2020.

#include "Alchemist.h"

#include <cassert>

int main(int argc, char* argv[])
{
	printf("Log statements work\n");
	
	Alchemist AlchemistInstance = Alchemist();

	AlchemistInstance.Run();

	return 0;
}

#if IS_WEB
EM_JS(int, GetWindowWidthJS, (), {
	return window.innerWidth;
});

EM_JS(int, GetWindowHeightJS, (), {
	return window.innerHeight;
});

void LoopCallback(void* Arg)
{
	((Alchemist*)Arg)->Frame();
}

EM_BOOL UiEventCallback(int Type, const EmscriptenUiEvent* UiEvent, void* UserData)
{
	return ((Alchemist*)UserData)->UiEvent(Type, UiEvent);
}
#endif


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


Alchemist::Alchemist()
{
	// Initialize SDL
	assert(SDL_Init(SDL_INIT_VIDEO) == 0);
	assert(SDL_CreateWindowAndRenderer(_GetWindowStartSize().X, _GetWindowStartSize().Y, 0, &Window, &Renderer) == 0);
	
	SDL_SetWindowResizable(Window, SDL_TRUE);
	
	// Get window surface
	WindowSurface = SDL_GetWindowSurface(Window);

	// Load resources
	Font = TTF_OpenFont("Resources/Font.ttf", 24);
	TestImageSurface = IMG_Load("Resources/arobase.png");

	TestImageTexture = SDL_CreateTextureFromSurface(Renderer, TestImageSurface);
}

Alchemist::~Alchemist()
{
	// Destroy window
	SDL_DestroyWindow(Window);

	// Quit SDL subsystems
	SDL_Quit();
}

void Alchemist::Run()
{
#if !IS_WEB
	while(!Close)
	{
		Frame();
		SDL_Delay(16);
	}
#else
	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, UiEventCallback);
	emscripten_set_main_loop_arg(&LoopCallback, this, 60, 1);
#endif
}

void Alchemist::Frame()
{
	SDL_Event Event;

	//Handle events on queue
	while (SDL_PollEvent(&Event))
	{
		switch (Event.type)
		{
			case SDL_QUIT:
			{
				Close = true;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if (Event.button.button == 2)
				{
					printf("Drag start\n");
					ViewDrag = true;
				}

				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				if (Event.button.button == 2)
				{
					printf("Drag stop\n");
					ViewDrag = false;
				}

				break;
			}
			case SDL_MOUSEMOTION:
			{
				if (ViewDrag)
				{
					ViewTopLeft.X -= Event.motion.xrel;
					ViewTopLeft.Y -= Event.motion.yrel;
				}

				break;
			}
		}
	}

	// Draw background
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	SDL_RenderClear(Renderer);

	// Draw grid
	int Width;
	int Height;

	SDL_GetWindowSize(Window, &Width, &Height);

	int SquareSize = 64;

	SDL_SetRenderDrawColor(Renderer, 220, 220, 220, 255);

	int x = -ViewTopLeft.X % SquareSize;
	int y = -ViewTopLeft.Y % SquareSize;

	for (; x < Width; x += SquareSize)
	{
		SDL_RenderDrawLine(Renderer, x, 0, x, Height);
	}

	for (; y < Height; y += SquareSize)
	{
		SDL_RenderDrawLine(Renderer, 0, y, Width, y);
	}

	// Draw sprite
	SDL_Rect DestRect = { -ViewTopLeft.X, -ViewTopLeft.Y, 200, 200 };

	SDL_RenderCopy(Renderer, TestImageTexture, NULL, &DestRect);

	// Finish
	SDL_RenderPresent(Renderer);
}

#if IS_WEB
EM_BOOL Alchemist::UiEvent(int Type, const EmscriptenUiEvent* UiEvent)
{
	SDL_SetWindowSize(Window, UiEvent->windowInnerWidth, UiEvent->windowInnerHeight);
	return EM_TRUE;
}
#endif

Size Alchemist::_GetWindowStartSize()
{
#if IS_WEB
	return { GetWindowWidthJS(), GetWindowHeightJS() };
#else
	return { 1600, 900 };
#endif
}
