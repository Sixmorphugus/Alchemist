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
	assert(SDL_CreateWindowAndRenderer(GetWindowStartWidth(), GetWindowStartHeight(), 0, &Window, &Renderer) == 0);
	
	SDL_SetWindowResizable(Window, SDL_TRUE);
	
	// Get window surface
	WindowSurface = SDL_GetWindowSurface(Window);

	// Load our background @ image
	LogoSurface = IMG_Load("Resources/arobase.png");
	assert(LogoSurface);

	LogoTexture = SDL_CreateTextureFromSurface(Renderer, LogoSurface);
}

Alchemist::~Alchemist()
{
	// Destroy background
	SDL_DestroyTexture(LogoTexture);
	SDL_FreeSurface(LogoSurface);
	
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

int Alchemist::GetWindowStartWidth()
{
#if IS_WEB
	return GetWindowWidthJS();
#endif
	
	return 1600;
}

int Alchemist::GetWindowStartHeight()
{
#if IS_WEB
	return GetWindowHeightJS();
#endif
	
	return 900;
}

void Alchemist::_Loop()
{
	SDL_Event Event;
	
	//Handle events on queue
	while (SDL_PollEvent(&Event))
	{
		//User requests quit
		if (Event.type == SDL_QUIT)
		{
			LoopingInstance->Close = true;
		}
	}

	// Draw
	SDL_SetRenderDrawColor(LoopingInstance->Renderer, 255, 255, 255, 255);
	SDL_RenderClear(LoopingInstance->Renderer);

	int Width;
	int Height;

	SDL_GetWindowSize(LoopingInstance->Window, &Width, &Height);
	
	for (int x = 0; x < Width; x += 300)
	{
		for (int y = 0; y < Height; y += 300)
		{
			SDL_Rect DestRect = { x, y, 300, 300 };
			SDL_RenderCopy(LoopingInstance->Renderer, LoopingInstance->LogoTexture, NULL, &DestRect);
		}
	}

	SDL_RenderPresent(LoopingInstance->Renderer);
}

#if IS_WEB
EM_BOOL Alchemist::_UiEvent(int Type, const EmscriptenUiEvent* UiEvent, void* UserData)
{
	SDL_SetWindowSize(LoopingInstance->Window, UiEvent->windowInnerWidth, UiEvent->windowInnerHeight);
	return EM_TRUE;
}
#endif