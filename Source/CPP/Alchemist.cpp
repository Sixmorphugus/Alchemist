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
#endif


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


Alchemist* LoopingInstance = nullptr;


Alchemist::Alchemist()
{
	// Initialize SDL
	assert(SDL_Init(SDL_INIT_VIDEO) == 0);
	assert(SDL_CreateWindowAndRenderer(GetWindowStartSize().X, GetWindowStartSize().Y, 0, &Window, &Renderer) == 0);
	
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
	LoopingInstance = this;
	
#if !IS_WEB
	while(!Close)
	{
		_Loop();
		SDL_Delay(16);
	}
#else
	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, _UiEvent);
	emscripten_set_main_loop(_Loop, 60, 1);
#endif
}

Size Alchemist::GetWindowStartSize()
{
#if IS_WEB
	return { GetWindowWidthJS(), GetWindowHeightJS() };
#else
	return { 1600, 900 };
#endif
}

void Alchemist::_Loop()
{
	SDL_Event Event;
	
	//Handle events on queue
	while (SDL_PollEvent(&Event))
	{
		switch(Event.type)
		{
			case SDL_QUIT:
			{
				LoopingInstance->Close = true;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if(Event.button.button == 3)
				{
					printf("Drag start\n");
					LoopingInstance->ViewDrag = true;
				}
				
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				if (Event.button.button == 3)
				{
					printf("Drag stop\n");
					LoopingInstance->ViewDrag = false;
				}
				
				break;
			}
			case SDL_MOUSEMOTION:
			{
				if(LoopingInstance->ViewDrag)
				{
					LoopingInstance->ViewTopLeft.X -= Event.motion.xrel;
					LoopingInstance->ViewTopLeft.Y -= Event.motion.yrel;
				}
				
				break;
			}
		}
	}

	// Draw background
	SDL_SetRenderDrawColor(LoopingInstance->Renderer, 255, 255, 255, 255);
	SDL_RenderClear(LoopingInstance->Renderer);

	// Draw grid
	int Width;
	int Height;

	SDL_GetWindowSize(LoopingInstance->Window, &Width, &Height);

	int SquareSize = 64;
	
	SDL_SetRenderDrawColor(LoopingInstance->Renderer, 220, 220, 220, 255);

	int x = (LoopingInstance->ViewTopLeft.X >= 0 ? -abs(LoopingInstance->ViewTopLeft.X) : abs(LoopingInstance->ViewTopLeft.X)) % SquareSize;
	int y = (LoopingInstance->ViewTopLeft.Y >= 0 ? -abs(LoopingInstance->ViewTopLeft.Y) : abs(LoopingInstance->ViewTopLeft.Y)) % SquareSize;
	
	for (; x < Width; x += SquareSize)
	{
		SDL_RenderDrawLine(LoopingInstance->Renderer, x, 0, x, Height);
	}

	for (; y < Height; y += SquareSize)
	{
		SDL_RenderDrawLine(LoopingInstance->Renderer, 0, y, Width, y);
	}

	// Draw sprite
	SDL_Rect DestRect = {-LoopingInstance->ViewTopLeft.X, -LoopingInstance->ViewTopLeft.Y, 200, 200};
	
	SDL_RenderCopy(LoopingInstance->Renderer, LoopingInstance->TestImageTexture, NULL, &DestRect);

	// Finish
	SDL_RenderPresent(LoopingInstance->Renderer);
}

#if IS_WEB
EM_BOOL Alchemist::_UiEvent(int Type, const EmscriptenUiEvent* UiEvent, void* UserData)
{
	SDL_SetWindowSize(LoopingInstance->Window, UiEvent->windowInnerWidth, UiEvent->windowInnerHeight);
	return EM_TRUE;
}
#endif