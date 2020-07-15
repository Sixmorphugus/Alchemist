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
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);

	// Load resources
	Resources.LoadResources(this);
	
	// Get window surface
	WindowSurface = SDL_GetWindowSurface(Window);

	// Create a node
	CreateNode<Node_Root>(Point(0, 0));
	CreateNode<Node_Root>(Point(1, 0));
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
				if (Event.button.button == 3)
				{
					ViewDrag = true;
				}
				else if(Event.button.button == 1)
				{
					// Look for a node under the mouse.
					// If there is one, begin dragging it.
					Point MouseGridPosition = ScreenToGrid(Point(Event.motion.x, Event.motion.y));
					NodeOnMouse = GetNodeAt(MouseGridPosition);
				}

				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				if (Event.button.button == 3)
				{
					ViewDrag = false;
				}
				else if (Event.button.button == 1)
				{
					// If there is a node on the mouse, and the grid space below the mouse is clear, insert the node there.
					if (NodeOnMouse)
					{
						Point MouseGridPosition = ScreenToGrid(Point(Event.motion.x, Event.motion.y));
						PlaceNode(NodeOnMouse, MouseGridPosition);

						NodeOnMouse.reset();
					}
				}

				break;
			}
			case SDL_MOUSEMOTION:
			{
				MousePos.X = Event.motion.x;
				MousePos.Y = Event.motion.y;

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
		shared_ptr<Node> NodeOnGrid = NodesOnGrid[i];

		// Grid position to screen position
		Point ScreenPosition = GridToScreen(Position);

		// Draw
		NodeOnGrid->Draw(this, ScreenPosition, NodeOnGrid == NodeOnMouse);
	}

	// Draw node being dragged
	if (NodeOnMouse)
	{
		// Show a transparent preview of the node being dragged;
		// Hilight the grid space hovered in blue if empty, or red if occupied.
		// If the hovered grid space is empty, lock the node to it, otherwise move it exactly to the position of the mouse.
		Point DrawPos = MousePos;

		Point MouseGridPosition = ScreenToGrid(MousePos);
		Point GridPos = GridToScreen(MouseGridPosition);
		shared_ptr<Node> NodeUnderMouse = GetNodeAt(MouseGridPosition);

		if (!NodeUnderMouse || NodeUnderMouse == NodeOnMouse)
		{
			DrawPos = GridPos;
			SDL_SetRenderDrawColor(Renderer, 100, 100, 255, 100);
		}
		else
		{
			SDL_SetRenderDrawColor(Renderer, 255, 100, 100, 100);
		}

		SDL_Rect Rect{GridPos.X, GridPos.Y, GridSize, GridSize};

		SDL_RenderFillRect(Renderer, &Rect);

		NodeOnMouse->Draw(this, DrawPos, true);
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

bool Alchemist::PlaceNode(shared_ptr<Node> NewNode, const Point& Position)
{
	if (GetNodeAt(Position))
	{
		// Space is occupied already
		return false;
	}

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
				return true;
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

	return true;
}

shared_ptr<Node> Alchemist::GetNodeAt(const Point& Position) const
{
	auto Found = GridLookup.find(Position);

	if(Found != GridLookup.end())
	{
		return GetNode(Found->second);
	}

	return nullptr;
}

shared_ptr<Node> Alchemist::GetNode(int ID) const
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
