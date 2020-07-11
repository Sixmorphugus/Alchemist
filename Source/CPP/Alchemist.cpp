// Copyright Chris Sixsmith 2020.

#include "Alchemist.h"

#include "Nodes/Special/Node_Root.h"

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

	// Load resources
	Resources.LoadResources(this);
	
	// Get window surface
	WindowSurface = SDL_GetWindowSurface(Window);

	// Create a node
	CreateNode<Node_Root>(Point(0, 0));
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

	SDL_SetRenderDrawColor(Renderer, 220, 220, 220, 255);

	int x = -ViewTopLeft.X % GridSize;
	int y = -ViewTopLeft.Y % GridSize;

	for (; x < Width; x += GridSize)
	{
		SDL_RenderDrawLine(Renderer, x, 0, x, Height);
	}

	for (; y < Height; y += GridSize)
	{
		SDL_RenderDrawLine(Renderer, 0, y, Width, y);
	}

	// If 0, 0 is on screen, draw it
	SDL_SetRenderDrawColor(Renderer, 150, 150, 150, 255);
	
	SDL_RenderDrawLine(Renderer, ViewTopLeft.X, -ViewTopLeft.Y, Width, -ViewTopLeft.Y);
	SDL_RenderDrawLine(Renderer, -ViewTopLeft.X, ViewTopLeft.Y, -ViewTopLeft.X, Height);

	// Draw nodes
	for(int i = 0; i < (int)NodesOnGrid.size(); i++)
	{
		Point Position = GridReverseLookup[i];
		Node* NodeOnGrid = NodesOnGrid[i];

		// Grid position to screen position
		Point ScreenPosition = GridToScreen(Position);

		// Draw
		NodeOnGrid->Draw(this, ScreenPosition);
	}

	// Finish
	SDL_RenderPresent(Renderer);
}

Point Alchemist::ScreenToGraph(const Point& ScreenPosition)
{
	return ScreenPosition + ViewTopLeft;
}

Point Alchemist::GraphToScreen(const Point& GraphPosition)
{
	return GraphPosition - ViewTopLeft;
}

Point Alchemist::GraphToGrid(const Point& GraphPosition)
{
	return GraphPosition / GridSize;
}

Point Alchemist::GridToGraph(const Point& GridPosition)
{
	return GridPosition * GridSize;
}

Point Alchemist::GridToScreen(const Point& GridPosition)
{
	return GraphToScreen(GridToGraph(GridPosition));
}

Point Alchemist::ScreenToGrid(const Point& ScreenPosition)
{
	return GraphToGrid(ScreenToGraph(ScreenPosition));
}

#if IS_WEB
EM_BOOL Alchemist::UiEvent(int Type, const EmscriptenUiEvent* UiEvent)
{
	SDL_SetWindowSize(Window, UiEvent->windowInnerWidth, UiEvent->windowInnerHeight);
	return EM_TRUE;
}
#endif

void Alchemist::PlaceNode(Node* NewNode, const Point& Position)
{
	// Is this node on the grid already? (i.e. being moved)
	int Found = -1;
	
	for(int i = 0; i < NodesOnGrid.size(); i++)
	{
		if(NodesOnGrid[i] == NewNode)
		{
			// We need to remove the lookup table entries.
			Point PreviousPosition = GridReverseLookup[i];

			if(PreviousPosition == Position)
			{
				// Stop! Do nothing if the node is where we want it already.
				return;
			}
			
			GridReverseLookup.erase(GridReverseLookup.find(i));
			GridLookup.erase(GridLookup.find(PreviousPosition));

			Found = i;
			
			break;
		}
	}

	// Add if not found
	if(Found == -1)
	{
		NodesOnGrid.push_back(NewNode);
		Found = (int)NodesOnGrid.size() - 1;
	}

	// Add to lookups
	GridLookup[Position] = Found;
	GridReverseLookup[Found] = Position;
}

Node* Alchemist::GetNodeAt(const Point& Position) const
{
	auto Found = GridLookup.find(Position);

	if(Found != GridLookup.end())
	{
		return GetNode(Found->second);
	}

	return nullptr;
}

Node* Alchemist::GetNode(int ID) const
{
	return NodesOnGrid[ID];
}

void Alchemist::RemoveNode(int ID)
{
	// TODO
}

Size Alchemist::_GetWindowStartSize()
{
#if IS_WEB
	return { GetWindowWidthJS(), GetWindowHeightJS() };
#else
	return { 1600, 900 };
#endif
}
