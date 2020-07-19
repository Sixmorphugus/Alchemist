// Copyright Chris Sixsmith 2020.

#include "Alchemist.h"
#include "Module/Function.h"

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
	: CurrentModule(this)
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
	CurrentFunction = CurrentModule.CreateOrGetFunction("main", 0);

	CurrentFunction->CreateNode<Node_Root>(Point(0, 0));
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
	assert(CurrentFunction);

	int Width;
	int Height;

	SDL_GetWindowSize(Window, &Width, &Height);

	vector<Category> CategorisedNodes = Nodes.GetCategorisedNodes(); // todo: constantly calling this is terrible!
	const Category& CurrentCategory = CategorisedNodes[PaletteCategory];


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////


	// Calculate the category item the mouse is currently over (null if none)
	int PaletteSelection = -1;

	for (int i = 0; i < CurrentCategory.Nodes.size(); i++)
	{
		Point NodePos = GetPaletteItemPosition(i);
		if (MousePos.X > NodePos.X&& MousePos.Y > NodePos.Y&& MousePos.X <= NodePos.X + GridSize && MousePos.Y <= NodePos.Y + GridSize)
		{
			PaletteSelection = i;
		}
	}


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////


	//Handle events on queue
	{
		SDL_Event Event;

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
					if (Event.motion.x < Width - SidebarWidth)
					{
						if (Event.button.button == 3)
						{
							ViewDrag = true;
						}
						else if (Event.button.button == 1)
						{
							// Look for a node under the mouse.
							// If there is one, begin dragging it.
							Point MouseGridPosition = ScreenToGrid(Point(Event.motion.x, Event.motion.y));
							NodeOnMouse = CurrentFunction->GetNodeAt(MouseGridPosition);
						}
					}
					else
					{
						if (PaletteSelection != -1)
						{
							NodeOnMouse = CurrentCategory.Nodes[PaletteSelection]->Clone();
						}
					}

					break;
				}
				case SDL_MOUSEBUTTONUP:
				{
					if (Event.motion.x < Width - SidebarWidth)
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
								CurrentFunction->PlaceNode(NodeOnMouse, MouseGridPosition);

								NodeOnMouse.reset();
							}
						}
					}
					else
					{
						if (NodeOnMouse)
						{
							CurrentFunction->RemoveNode(NodeOnMouse);
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
	}


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////


	// Draw background
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	SDL_RenderClear(Renderer);

	// Draw grid
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


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////


	// Draw nodes
	for(shared_ptr<Node> NodeOnGrid : CurrentFunction->GetNodesOnGrid())
	{
		Point Position = NodeOnGrid->GetGridPosition();

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

		if (MousePos.X < Width - SidebarWidth)
		{
			Point MouseGridPosition = ScreenToGrid(MousePos);
			Point GridPos = GridToScreen(MouseGridPosition);
			shared_ptr<Node> NodeUnderMouse = CurrentFunction->GetNodeAt(MouseGridPosition);

			if (!NodeUnderMouse || NodeUnderMouse == NodeOnMouse)
			{
				DrawPos = GridPos;
				SDL_SetRenderDrawColor(Renderer, 100, 100, 255, 100);
			}
			else
			{
				SDL_SetRenderDrawColor(Renderer, 255, 100, 100, 100);
			}

			SDL_Rect Rect{ GridPos.X, GridPos.Y, GridSize, GridSize };

			SDL_RenderFillRect(Renderer, &Rect);
		}
		else
		{
			// TODO somehow show that releasing the node here will delete it?
		}

		NodeOnMouse->Draw(this, DrawPos, true);
	}


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////


	// Render palette
	SDL_Rect PaletteRect{ Width - SidebarWidth, 0, SidebarWidth, Height };

	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 100);
	SDL_RenderFillRect(Renderer, &PaletteRect);

	// Render items in selected palette category
	for (int i = 0; i < CurrentCategory.Nodes.size(); i++)
	{
		if (i == PaletteSelection)
		{
			Point SelectionPos = GetPaletteItemPosition(i);

			SDL_Rect Rect{ SelectionPos.X - 2, SelectionPos.Y - 2, GridSize + 4, GridSize + 4 };

			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 100);
			SDL_RenderFillRect(Renderer, &Rect);
		}

		CurrentCategory.Nodes[i]->Draw(this, GetPaletteItemPosition(i));
	}

	// Render palette tabs
	for (int i = 0; i < CategorisedNodes.size(); i++)
	{
		Point CategoryButtonPos = GetCategoryButtonPosition(i);

		SDL_Rect Rect{ CategoryButtonPos.X, CategoryButtonPos.Y, GridSize, GridSize };

		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, i == PaletteCategory ? 100 : 50);
		SDL_RenderFillRect(Renderer, &Rect);

		if (i == PaletteCategory)
		{
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

			Rect.w = 2;
			SDL_RenderFillRect(Renderer, &Rect);

			Rect.w = GridSize + 2;
			Rect.h = 2;

			if (i != 0)
			{
				SDL_RenderFillRect(Renderer, &Rect);
			}

			Rect.y += GridSize - 2;
			SDL_RenderFillRect(Renderer, &Rect);
		}

		CategorisedNodes[i].Nodes[0]->Draw(this, CategoryButtonPos);
	}

	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

	PaletteRect.w = 2;

	// top half of palette border
	PaletteRect.h = GetCategoryButtonPosition(PaletteCategory).Y - 1;
	SDL_RenderFillRect(Renderer, &PaletteRect);

	// bottom half of palette border
	PaletteRect.y = GetCategoryButtonPosition(PaletteCategory).Y + GridSize;
	PaletteRect.h = Height - GetCategoryButtonPosition(PaletteCategory).Y + GridSize;
	SDL_RenderFillRect(Renderer, &PaletteRect);


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////


	// Finish
	SDL_RenderPresent(Renderer);
}

Point Alchemist::ScreenToGraph(const Point& ScreenPosition) const
{
	return ScreenPosition + ViewTopLeft;
}

Point Alchemist::GraphToScreen(const Point& GraphPosition) const
{
	return GraphPosition - ViewTopLeft;
}

Point Alchemist::GraphToGrid(const Point& GraphPosition) const
{
	return GraphPosition / GridSize;
}

Point Alchemist::GridToGraph(const Point& GridPosition) const
{
	return GridPosition * GridSize;
}

Point Alchemist::GridToScreen(const Point& GridPosition) const
{
	return GraphToScreen(GridToGraph(GridPosition));
}

Point Alchemist::ScreenToGrid(const Point& ScreenPosition) const
{
	return GraphToGrid(ScreenToGraph(ScreenPosition));
}

Point Alchemist::GetPaletteItemPosition(int Index) const
{
	Point Out
	{
		 GetWindowSize().X + SidebarPadding - SidebarWidth + ((SidebarWidth / SidebarItemCount) * (Index % SidebarItemCount)),
		 SidebarPadding + ((SidebarWidth / SidebarItemCount) * (Index / SidebarItemCount))
	};
	
	return Out;
}

Point Alchemist::GetCategoryButtonPosition(int Index) const
{
	Point Out
	{
		 GetWindowSize().X - SidebarWidth - GridSize,
		 Index * GridSize
	};

	return Out;
}

#if IS_WEB
EM_BOOL Alchemist::UiEvent(int Type, const EmscriptenUiEvent* UiEvent)
{
	SDL_SetWindowSize(Window, UiEvent->windowInnerWidth, UiEvent->windowInnerHeight);
	return EM_TRUE;
}
#endif

Size Alchemist::GetWindowSize() const
{
	Size Out;

	SDL_GetWindowSize(Window, &Out.X, &Out.Y);

	return Out;
}

Size Alchemist::_GetWindowStartSize() const
{
#if IS_WEB
	return { GetWindowWidthJS(), GetWindowHeightJS() };
#else
	return { 1600, 900 };
#endif
}
