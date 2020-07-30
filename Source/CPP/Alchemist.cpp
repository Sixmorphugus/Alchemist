// Copyright Chris Sixsmith 2020.

#include "Alchemist.h"

#include "DrawShapes.h"
#include "Module/Function.h"

#include "Nodes/Special/Node_Root.h"
#include "Resources/Resource_Font.h"

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
	assert(SDL_CreateWindowAndRenderer(GetWindowStartSize().X, GetWindowStartSize().Y, 0, &Window, &Renderer) == 0);

	assert(TTF_Init() == 0);
	
	SDL_SetWindowResizable(Window, SDL_TRUE);
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);

	// Load resources
	Resources.LoadResources(this);
	
	// Get window surface
	WindowSurface = SDL_GetWindowSurface(Window);

	// Create a node
	CurrentFunction = CurrentModule.CreateOrGetFunction("main", 0);
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

	shared_ptr<Resource_Font> Font = Resources.GetResource<Resource_Font>("Font.ttf");
	

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
					if (Event.motion.x < Width - SidebarWidth - GridSize)
					{
						Point EventMousePosition(Event.motion.x, Event.motion.y);
						
						Point MouseGridPosition = ScreenToGrid(EventMousePosition);
						
						if (Event.button.button == 2)
						{
							ViewDrag = true;
						}
						else if (Event.button.button == 1)
						{
							if (NodeBeingConnected)
							{
								if(NodeBeingConnectedTo)
								{
									// An options menu is open near this node.
									// Find if we just clicked any option.
									// The position of the menu options is fairly easy to work out.
									Point OptionPosition = GetOptionsMenuPosition(NodeBeingConnectedTo);
									Size OptionSize = GetOptionsMenuOptionSize(NodeBeingConnectedTo, Font);

									for(int i = 0; i < NodeBeingConnectedTo->GetNumArguments(); i++)
									{
										SDL_Rect OptionRect = {
											OptionPosition.X,
											OptionPosition.Y + (ConnectMenuOptionPadding / 2) + (OptionSize.Y * i),
											OptionSize.X,
											OptionSize.Y
										};

										if(EventMousePosition.IsInRectangle(OptionRect))
										{
											// Make a connection between arg i and the node, being connected, then break.
											NodeBeingConnectedTo->SetConnector(NodeBeingConnected, i);
											break;
										}
									}
								}
								
								// Kill any pending connection.
								NodeBeingConnected.reset();
								NodeBeingConnectedTo.reset();
							}
							
							// Look for a node under the mouse.
							// If there is one, begin dragging it.
							NodeOnMouse = CurrentFunction->GetNodeAt(MouseGridPosition);
						}
						else if(Event.button.button == 3 && !NodeOnMouse)
						{
							// Look for a node under the mouse.
							// If there is one, start creating a connector from this node.
							NodeBeingConnected = CurrentFunction->GetNodeAt(MouseGridPosition);
						}
					}
					else
					{
						// Pick up the palette selection.
						if (PaletteSelection != -1)
						{
							NodeOnMouse = CurrentCategory.Nodes[PaletteSelection]->Clone();
						}
						else
						{
							// If there is no palette selection, look for the mouse being over a category button
							if(Event.motion.x < Width - SidebarWidth && Event.motion.x > Width - SidebarWidth - GridSize)
							{
								int Category = Event.motion.y / GridSize;

								if(Category < CategorisedNodes.size())
								{
									PaletteCategory = Category;
								}
							}
						}
					}

					break;
				}
				case SDL_MOUSEBUTTONUP:
				{
					if (Event.motion.x < Width - SidebarWidth)
					{
						if (Event.button.button == 2)
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
						else if (Event.button.button == 3)
						{
							// If there is a connector being dragged, connect it to the node it was dropped on. Always stop dragging at this point
							if(NodeBeingConnected)
							{
								// If we dropped on another node, bring up the second stage connecting menu!
								Point MouseGridPosition = ScreenToGrid(Point(Event.motion.x, Event.motion.y));
								NodeBeingConnectedTo = CurrentFunction->GetNodeAt(MouseGridPosition);

								if(!NodeBeingConnectedTo || NodeBeingConnectedTo == NodeBeingConnected)
								{
									NodeBeingConnected.reset();
									NodeBeingConnectedTo.reset();
								}
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

	// Draw node connections
	for (shared_ptr<Node> NodeOnGrid : CurrentFunction->GetNodesOnGrid())
	{
		for(int i = 0; i < NodeOnGrid->GetNumArguments(); i++)
		{
			if(shared_ptr<Node> Connector = NodeOnGrid->GetConnector(i))
			{
				SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
				DrawConnectorArrowOnGrid(Connector->GetGridPosition(), NodeOnGrid->GetGridPosition());
			}
		}
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
	else
	{
		// Draw node captions of any node under the mouse
		Point MouseGridPosition = ScreenToGrid(MousePos);
		shared_ptr<Node> NodeUnderMouse = CurrentFunction->GetNodeAt(MouseGridPosition);

		if(NodeUnderMouse)
		{
			string NodeDetailText = NodeUnderMouse->GetDisplayName();

			SDL_Texture* DisplayTexture = Font->GetStringTexture(NodeDetailText);
			Size DisplaySize = Font->GetStringScreenSize(NodeDetailText);

			SDL_Rect DisplayRect;
			
			DisplayRect.x = MousePos.X + 20;
			DisplayRect.y = MousePos.Y + 20;
			DisplayRect.w = DisplaySize.X;
			DisplayRect.h = DisplaySize.Y;

			SDL_SetTextureColorMod(DisplayTexture, 0, 0, 0);
			SDL_RenderCopy(Renderer, DisplayTexture, NULL, &DisplayRect);
		}
	}

	// Draw connector being dragged
	if(NodeBeingConnected)
	{
		if(!NodeBeingConnectedTo) // de-facto "dragging" state
		{
			Point MouseGridPosition = ScreenToGrid(MousePos);
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 200);
			DrawConnectorArrowOnGrid(NodeBeingConnected->GetGridPosition(), MouseGridPosition);
		}
		else // de-facto "connecting" state
		{
			SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 255);
			DrawConnectorArrowOnGrid(NodeBeingConnected->GetGridPosition(), NodeBeingConnectedTo->GetGridPosition());

			Point OptionPosition = GetOptionsMenuPosition(NodeBeingConnected);
			Size OptionSize = GetOptionsMenuOptionSize(NodeBeingConnectedTo, Font);

			SDL_Rect OptionsRect = {
				OptionPosition.X,
				OptionPosition.Y,
				OptionSize.X,
				OptionSize.Y
			};
			
			// Turn per-option size into full menu size.
			OptionsRect.h *= NodeBeingConnectedTo->GetNumArguments();

			// Add padding
			OptionsRect.w += ConnectMenuOptionPadding;
			OptionsRect.h += ConnectMenuOptionPadding;
			
			// Draw pop-out variable list
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 200);
			SDL_RenderFillRect(Renderer, &OptionsRect);

			// Draw outline (x2)
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
			DrawThickRectangle(this, OptionsRect, 2);

			for (int i = 0; i < NodeBeingConnectedTo->GetNumArguments(); i++)
			{
				// For rendering string
				string Argument = NodeBeingConnectedTo->GetArgumentName(i);

				Size StringSize = Font->GetStringScreenSize(Argument);

				SDL_Rect Rect = {
					OptionsRect.x + (ConnectMenuOptionPadding / 2),
					OptionsRect.y + (ConnectMenuOptionPadding / 2) + (OptionSize.Y * i),
					StringSize.X,
					StringSize.Y
				};

				// For rendering highlight
				SDL_Rect HighlightRect = {
					OptionsRect.x,
					OptionsRect.y + (ConnectMenuOptionPadding / 2) + (OptionSize.Y * i),
					OptionsRect.w,
					OptionSize.Y
				};

				// Render highlight first, IF the mouse is within its area.
				if(MousePos.IsInRectangle(HighlightRect))
				{
					SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 100);
					SDL_RenderFillRect(Renderer, &HighlightRect);
				}

				// Then string
				SDL_RenderCopy(Renderer, Font->GetStringTexture(Argument), NULL, &Rect);
			}
		}
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

Size Alchemist::GetOptionsMenuPosition(const shared_ptr<Node>& NodeOnGrid) const
{
	return Size{ GridToScreen(NodeBeingConnectedTo->GetGridPosition() + Point(1, 1)).X + 2, GridToScreen(NodeBeingConnectedTo->GetGridPosition() + Point(1, 1)).Y + 2 };
}

Size Alchemist::GetOptionsMenuOptionSize(const shared_ptr<Node>& NodeOnGrid, const shared_ptr<Resource_Font>& FontResource) const
{
	Size SizeOut;

	for (int i = 0; i < NodeOnGrid->GetNumArguments(); i++)
	{
		string Argument = NodeOnGrid->GetArgumentName(i);

		Size StringSize = FontResource->GetStringScreenSize(Argument);

		if (StringSize.X > SizeOut.X)
		{
			SizeOut.X = StringSize.X;
		}

		if (StringSize.Y > SizeOut.Y)
		{
			SizeOut.Y = StringSize.Y;
		}
	}

	return SizeOut;
}

Size Alchemist::GetWindowStartSize() const
{
#if IS_WEB
	return { GetWindowWidthJS(), GetWindowHeightJS() };
#else
	return { 1600, 900 };
#endif
}

void Alchemist::DrawConnectorArrowOnGrid(const Point& Point1, const Point& Point2)
{
	Point ScreenPoint1 = GridToScreen(Point1) + (GridSize / 2);
	Point ScreenPoint2 = GridToScreen(Point2) + (GridSize / 2);

	// We need to translate the ends of the arrow out of their circles.
	// First find the unit vector of the line.
	double Radius = 40.f;
	Point PushVector = (Point2 - Point1).NormaliseToLength(Radius);

	ScreenPoint1 += PushVector;
	ScreenPoint2 -= PushVector;
	
	DrawConnectorArrow(this, ScreenPoint1, ScreenPoint2);
}
