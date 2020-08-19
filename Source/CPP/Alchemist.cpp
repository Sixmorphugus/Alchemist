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
	return document.getElementById("canvas").width;
});

EM_JS(int, GetWindowHeightJS, (), {
	return document.getElementById("canvas").height; 
});

EM_JS(void, EmitText, (const char* Text, int TextLen), {
	return document.getElementById("output").value = UTF8ToString(Text, TextLen);
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
	: CurrentModule(this), Nodes(this)
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
	CurrentFunction = CurrentModule.CreateOrGetFunction("Main", 0);
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

void Alchemist::Compile()
{
	string OutCode;
	vector<CompilationProblem> OutProblems;

	string OutPrint = "--- COMPILE RESULT ---\n\n";

	bool Ok = true;
	auto Functions = CurrentModule.GetFunctions();
	
	for(int i = 0; i < Functions.size(); i++)
	{
		if(!Functions[i]->Emit(OutCode, OutProblems))
		{
			Ok = false;
		}

		OutCode += "\n";
	}
	
	if (Ok)
	{
		// Print the code
		OutPrint += OutCode + "\n\n";
	}
	else
	{
		OutPrint += "FAILED\n\n";
	}

	// Always print the problems
	if (OutProblems.size() > 0)
	{
		OutPrint += "--- PROBLEMS ---\n\n";

		for (int i = 0; i < OutProblems.size(); i++)
		{
			// sorry god
			Function* ProblemFunction = OutProblems[i].ProblemNode->GetFunction();
			OutPrint += 
				"- (In " + (ProblemFunction->GetName() + ":" + to_string(ProblemFunction->GetArity()))
			  + " @ (" + to_string(OutProblems[i].ProblemNode->GetGridPosition().X) + ", " + to_string(OutProblems[i].ProblemNode->GetGridPosition().Y)
			  + ")) " + OutProblems[i].Problem + "\n";
		}
	}

#if IS_WEB
	EmitText(OutPrint.c_str(), OutPrint.size());
#endif
	
	// Leave the problems cached - we will display them until the code is recompiled, or a different function is opened.
	// TODO do this per function?
	ProblemsFromLastCompile = OutProblems;
}

void Alchemist::Frame()
{
	assert(CurrentFunction);


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////


	//Handle events on queue
	{
		SDL_Event Event;

		while (SDL_PollEvent(&Event))
		{
			// Core events
			switch (Event.type)
			{
				case SDL_QUIT:
				{
					Close = true;
					break;
				}
			}
			
			if (!ToolbarHandleEvent(Event))
			{
				if (!PaletteHandleEvent(Event))
				{
					GridHandleEvent(Event);
				}
			}

			Compile();
		}
	}

	DrawGrid();
	DrawNodePalette();
	DrawToolbar();
	
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
		 SidebarPadding + ((SidebarWidth / SidebarItemCount) * (Index / SidebarItemCount)) + ToolbarHeight + (ToolbarPadding * 2)
	};
	
	return Out;
}

Point Alchemist::GetCategoryButtonPosition(int Index) const
{
	Point Out
	{
		 GetWindowSize().X - SidebarWidth - GridSize,
		 (Index * GridSize) + ToolbarHeight + (ToolbarPadding * 2)
	};

	return Out;
}

#if IS_WEB
EM_BOOL Alchemist::UiEvent(int Type, const EmscriptenUiEvent* UiEvent)
{
	SDL_SetWindowSize(Window, GetWindowWidthJS(), GetWindowHeightJS());
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

Size Alchemist::GetOptionsMenuOptionSize(const shared_ptr<Node>& NodeOnGrid) const
{
	shared_ptr<Resource_Font> FontResource = GetDefaultFont();
	
	Size SizeOut;

	for (int i = 0; i < NodeOnGrid->GetNumArguments(); i++)
	{
		string Argument = to_string(i+1) + "  " + NodeOnGrid->GetArgumentName(i);

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

void Alchemist::ViewFunction(string Name)
{
	EditingFunctionSignature = false;
	
	shared_ptr<Function> Func = CurrentModule.GetFunction(Name);

	if(Func)
	{
		CurrentFunction = Func;
	}
}

void Alchemist::EditFunctionSignature()
{
	EditingFunctionSignature = true;
}

Size Alchemist::GetWindowStartSize() const
{
#if IS_WEB
	return { GetWindowWidthJS(), GetWindowHeightJS() };
#else
	return { 1600, 900 };
#endif
}

void Alchemist::DrawToolbar() const
{
	shared_ptr<Resource_Font> Font = GetDefaultFont();
	
	// Render background for toolbar in one go
	SDL_Rect ToolbarRect = {
		0,
		0,
		GetWindowSize().X,
		ToolbarHeight + (ToolbarPadding * 2)
	};
	
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 100);
	SDL_RenderFillRect(Renderer, &ToolbarRect);

	ToolbarRect.y = ToolbarHeight + (ToolbarPadding * 2) - 2;
	ToolbarRect.h = 2;
	
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &ToolbarRect);

	// Render logo
	Size LogoSize = Font->GetStringScreenSize("@");

	SDL_Rect OptionRect = {
		ToolbarPadding * 2,
		ToolbarPadding,
		LogoSize.X,
		LogoSize.Y
	};

	SDL_SetTextureColorMod(Font->GetStringTexture("@"), 0, 0, 0);
	SDL_RenderCopy(Renderer, Font->GetStringTexture("@"), NULL, &OptionRect);
	
	// Render individual buttons
	auto Options = GetToolbarOptions();

	for(int i = 0; i < (int)Options.size(); i++)
	{
		// Selection rectangle
		SDL_Rect OptionSelectionRect = GetToolbarOptionSelectionRect(i);

		int TextColorMod = 100;
		
		if (MousePos.IsInRectangle(OptionSelectionRect) || ToolbarOpenMenu == i)
		{
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 100);
			SDL_RenderFillRect(Renderer, &OptionSelectionRect);

			TextColorMod = 0;
		}

		// Text
		string OptionName = Options[i].Option;
		Size OptionSize = Font->GetStringScreenSize(OptionName);
		
		SDL_Rect OptionRect = {
			GetToolbarOptionX(i) + ToolbarPadding,
			ToolbarPadding,
			OptionSize.X,
			OptionSize.Y
		};

		SDL_SetTextureColorMod(Font->GetStringTexture(OptionName), TextColorMod, TextColorMod, TextColorMod);
		SDL_RenderCopy(Renderer, Font->GetStringTexture(OptionName), NULL, &OptionRect);
	}

	// Is a menu open?
	if(ToolbarOpenMenu != -1)
	{
		// If so, show a dropdown with that menu's options.
		for (int i = 0; i < (int)Options[ToolbarOpenMenu].SubOptions.size(); i++)
		{
			int TextColorMod = 100;
			int AlphaMod = 100;
			
			// Draw option rect
			SDL_Rect OptionRect = GetToolbarSubOptionRect(ToolbarOpenMenu, i);

			if (MousePos.IsInRectangle(OptionRect) && Options[ToolbarOpenMenu].SelectFunctions[i])
			{
				TextColorMod = 0;
				AlphaMod = 150;
			}
			
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, AlphaMod);
			SDL_RenderFillRect(Renderer, &OptionRect);
			
			// Draw option text
			string OptionName = Options[ToolbarOpenMenu].SubOptions[i];
			Size TextSize = Font->GetStringScreenSize(OptionName);

			SDL_Rect TextRect = {
				OptionRect.x + ToolbarPadding,
				OptionRect.y + ToolbarPadding,
				TextSize.X,
				TextSize.Y
			};
			
			SDL_SetTextureColorMod(Font->GetStringTexture(OptionName), TextColorMod, TextColorMod, TextColorMod);
			SDL_RenderCopy(Renderer, Font->GetStringTexture(OptionName), NULL, &TextRect);
		}
	}
}

void Alchemist::DrawNodePalette() const
{
	vector<Category> CategorisedNodes = Nodes.GetCategorisedNodes(); // todo: constantly calling this is terrible!
	const Category& CurrentCategory = CategorisedNodes[PaletteCategory];

	// Calculate the category item the mouse is currently over (null if none)
	int PaletteSelection = GetPaletteSelection(CurrentCategory);

	// Render palette
	SDL_Rect PaletteRect{ GetWindowSize().X - SidebarWidth, ToolbarHeight + (ToolbarPadding * 2), SidebarWidth, GetWindowSize().Y };

	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 100);
	SDL_RenderFillRect(Renderer, &PaletteRect);

	// Render items in selected palette category
	for (int i = 0; i < (int)CurrentCategory.Nodes.size(); i++)
	{
		if (i == PaletteSelection)
		{
			Point SelectionPos = GetPaletteItemPosition(i);

			SDL_Rect Rect{ SelectionPos.X - 2, SelectionPos.Y - 2, GridSize + 4, GridSize + 4 };

			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 100);
			SDL_RenderFillRect(Renderer, &Rect);

			// Draw node name
			DrawNodeTooltip(CurrentCategory.Nodes[i]);
		}

		CurrentCategory.Nodes[i]->Draw(this, GetPaletteItemPosition(i));
	}

	// Render palette tabs
	for (int i = 0; i < CategorisedNodes.size(); i++)
	{
		Point CategoryButtonPos = GetCategoryButtonPosition(i);

		SDL_Rect Rect{ CategoryButtonPos.X, CategoryButtonPos.Y, GridSize, GridSize };
		
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, i == PaletteCategory || MousePos.IsInRectangle(Rect) ? 100 : 50);
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
	PaletteRect.h = GetCategoryButtonPosition(PaletteCategory).Y - ToolbarHeight - (ToolbarPadding * 2);
	SDL_RenderFillRect(Renderer, &PaletteRect);

	// bottom half of palette border
	PaletteRect.y = GetCategoryButtonPosition(PaletteCategory).Y + GridSize;
	PaletteRect.h = GetWindowSize().Y - GetCategoryButtonPosition(PaletteCategory).Y + GridSize - ToolbarHeight - (ToolbarPadding * 2);
	SDL_RenderFillRect(Renderer, &PaletteRect);

	// Tooltip?
	for (int i = 0; i < CategorisedNodes.size(); i++)
	{
		Point CategoryButtonPos = GetCategoryButtonPosition(i);

		SDL_Rect Rect{ CategoryButtonPos.X, CategoryButtonPos.Y, GridSize, GridSize };

		if(MousePos.IsInRectangle(Rect))
		{
			DrawTooltip(CategorisedNodes[i].Name);
		}
	}
}

void Alchemist::DrawGrid() const
{
	shared_ptr<Resource_Font> Font = GetDefaultFont();
	
	// Draw background
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	SDL_RenderClear(Renderer);

	// Draw grid
	SDL_SetRenderDrawColor(Renderer, 220, 220, 220, 255);

	int x = -ViewTopLeft.X % GridSize;
	int y = -ViewTopLeft.Y % GridSize;

	for (; x < GetWindowSize().X; x += GridSize)
	{
		SDL_RenderDrawLine(Renderer, x, 0, x, GetWindowSize().Y);
	}

	for (; y < GetWindowSize().Y; y += GridSize)
	{
		SDL_RenderDrawLine(Renderer, 0, y, GetWindowSize().X, y);
	}

	// If 0, 0 is on screen, draw it
	SDL_SetRenderDrawColor(Renderer, 150, 150, 150, 255);

	SDL_RenderDrawLine(Renderer, ViewTopLeft.X, -ViewTopLeft.Y, GetWindowSize().X, -ViewTopLeft.Y);
	SDL_RenderDrawLine(Renderer, -ViewTopLeft.X, ViewTopLeft.Y, -ViewTopLeft.X, GetWindowSize().Y);
	
	// Draw nodes
	for (shared_ptr<Node> NodeOnGrid : CurrentFunction->GetNodes())
	{
		Point Position = NodeOnGrid->GetGridPosition();

		// Grid position to screen position
		Point ScreenPosition = GridToScreen(Position);

		SDL_Rect GridRect = {
			ScreenPosition.X,
			ScreenPosition.Y,
			GridSize,
			GridSize
		};
		
		if (NodeOnGrid == NodeLastSelected.lock())
		{
			// Draw selection rectangle.
			SDL_SetRenderDrawColor(Renderer, 200, 200, 200, 255);
			SDL_RenderFillRect(Renderer, &GridRect);
		}

		// Draw
		NodeOnGrid->Draw(this, ScreenPosition, NodeOnGrid == NodeOnMouse);
	}

	// Draw node connections
	for (shared_ptr<Node> NodeOnGrid : CurrentFunction->GetNodes())
	{
		for (int i = 0; i < NodeOnGrid->GetNumArguments(); i++)
		{
			if (shared_ptr<Node> Connector = NodeOnGrid->GetConnector(i))
			{
				SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

				if(NodeOnGrid->GetArgumentIsPattern(i))
				{
					SDL_SetRenderDrawColor(Renderer, 200, 0, 200, 255);
				}
				
				DrawConnectorArrowOnGrid(Connector->GetGridPosition(), NodeOnGrid->GetGridPosition(), i);
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

		if (MousePos.X < GetWindowSize().X - SidebarWidth)
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

		if (NodeUnderMouse && MousePos.X < GetWindowSize().X - SidebarWidth)
		{
			DrawNodeTooltip(NodeUnderMouse);
		}
	}

	// Draw connector being dragged
	if (NodeBeingConnected)
	{
		if (!NodeBeingConnectedTo) // de-facto "dragging" state
		{
			Point MouseGridPosition = ScreenToGrid(MousePos);
			shared_ptr<Node> NodeUnderMouse = CurrentFunction->GetNodeAt(MouseGridPosition);

			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 150);

			if (NodeUnderMouse)
			{
				if (NodeUnderMouse->GetNumArguments())
				{
					SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 150);
				}
				else
				{
					SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 150);
				}
			}

			DrawConnectorArrowOnGrid(NodeBeingConnected->GetGridPosition(), MouseGridPosition);
		}
		else // de-facto "connecting" state
		{
			SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 255);
			DrawConnectorArrowOnGrid(NodeBeingConnected->GetGridPosition(), NodeBeingConnectedTo->GetGridPosition());

			Point OptionPosition = GetOptionsMenuPosition(NodeBeingConnected);
			Size OptionSize = GetOptionsMenuOptionSize(NodeBeingConnectedTo);

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
				string Argument = to_string(i+1) + "  " + NodeBeingConnectedTo->GetArgumentName(i);

				Size StringSize = Font->GetStringScreenSize(Argument);

				SDL_Rect Rect = {
					OptionsRect.x + (ConnectMenuOptionPadding / 2),
					OptionsRect.y + (ConnectMenuOptionPadding / 2) + (OptionSize.Y * i),
					StringSize.X,
					StringSize.Y
				};

				SDL_Rect ConnectedRect = Rect;

				ConnectedRect.x -= 2;
				ConnectedRect.w = 20;

				// For rendering highlight
				SDL_Rect HighlightRect = {
					OptionsRect.x,
					OptionsRect.y + (ConnectMenuOptionPadding / 2) + (OptionSize.Y * i),
					OptionsRect.w,
					OptionSize.Y
				};

				// Render "connected" marking rectangle
				if (NodeBeingConnectedTo->GetConnector(i))
				{
					SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 100);
					SDL_RenderFillRect(Renderer, &ConnectedRect);
				}
				
				// Render highlight, IF the mouse is within its area.
				if (MousePos.IsInRectangle(HighlightRect))
				{
					SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 100);
					SDL_RenderFillRect(Renderer, &HighlightRect);
				}

				// Then string
				SDL_RenderCopy(Renderer, Font->GetStringTexture(Argument), NULL, &Rect);
			}
		}
	}

	// If editing function, show edit text
	if(EditingFunctionSignature)
	{
		string Text = CurrentFunction->GetName() + "_ : " + to_string(CurrentFunction->GetArity());
		string HintText = "[ENTER] Confirm[UP / DOWN] Change Arity";

		// Text
		Point Position = Point(ToolbarPadding, ToolbarHeight + (ToolbarPadding * 2));
		Size Siz = Font->GetStringScreenSize(Text);

		SDL_Rect Rect = {
			Position.X,
			Position.Y,
			Siz.X,
			Siz.Y
		};

		SDL_SetTextureColorMod(Font->GetStringTexture(Text), 0, 0, 0);
		SDL_RenderCopy(Renderer, Font->GetStringTexture(Text), NULL, &Rect);

		// HintText
		Siz = Font->GetStringScreenSize(HintText);

		Rect = {
			Position.X,
			Position.Y + 40,
			Siz.X,
			Siz.Y
		};

		SDL_SetTextureColorMod(Font->GetStringTexture(HintText), 0, 0, 0);
		SDL_RenderCopy(Renderer, Font->GetStringTexture(HintText), NULL, &Rect);
	}
}

bool Alchemist::ToolbarHandleEvent(SDL_Event& Event)
{
	switch(Event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
		{
			bool ClickedRootButton = false;
			
			if(Event.button.button == 1)
			{
				auto Options = GetToolbarOptions();

				// If the mouse is over a main toolbar option, open that toolbar menu.
				for (int i = 0; i < (int)Options.size(); i++)
				{
					SDL_Rect OptionRect = GetToolbarOptionSelectionRect(i);

					if(MousePos.IsInRectangle(OptionRect))
					{
						if(ToolbarOpenMenu != i)
						{
							ToolbarOpenMenu = i;
							ClickedRootButton = true;
						}
					}
				}

				// If we didn't just click a root button, it's possible we just clicked a menu button.
				if(!ClickedRootButton && ToolbarOpenMenu != -1)
				{
					for (int i = 0; i < (int)Options[ToolbarOpenMenu].SubOptions.size(); i++)
					{
						SDL_Rect OptionRect = GetToolbarSubOptionRect(ToolbarOpenMenu, i);

						if(Point(Event.button.x, Event.button.y).IsInRectangle(OptionRect))
						{
							// We have just clicked option i in menu ToolbarOpenMenu.
							// Call its on click function.
							if(Options[ToolbarOpenMenu].SelectFunctions[i])
							{
								Options[ToolbarOpenMenu].SelectFunctions[i](this);
							}
						}
					}
				}
			}

			if(!ClickedRootButton)
			{
				ToolbarOpenMenu = -1;
			}
			
			// If the mouse is over the toolbar, we always consume the mouse press.
			// Otherwise, the mouse press will be consumed if a menu is open.
			if(MousePos.Y < ToolbarHeight || ToolbarOpenMenu != -1)
			{
				return true;
			}
		}
		case SDL_TEXTINPUT:
		{
			if (EditingFunctionSignature)
			{
				CurrentFunction->Rename(CurrentFunction->GetName() + Event.text.text);
			}

			break;
		}
		case SDL_KEYDOWN:
		{
			if (EditingFunctionSignature)
			{
				if (Event.key.keysym.sym == SDLK_BACKSPACE)
				{
					CurrentFunction->Rename(CurrentFunction->GetName().substr(0, CurrentFunction->GetName().size() - 1));
				}
				else if (Event.key.keysym.sym == SDLK_UP)
				{
					CurrentFunction->SetArity(CurrentFunction->GetArity() + 1);
				}
				else if (Event.key.keysym.sym == SDLK_DOWN)
				{
					CurrentFunction->SetArity(CurrentFunction->GetArity() > 0 ? CurrentFunction->GetArity() - 1 : 0);
				}
				else if (Event.key.keysym.sym == SDLK_RETURN)
				{
					EditingFunctionSignature = false;
				}
			}

			break;
		}
	}

	if (EditingFunctionSignature)
	{
		return true;
	}
	
	return false;
}

bool Alchemist::PaletteHandleEvent(SDL_Event& Event)
{
	switch (Event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
		{
			if (Event.motion.x > GetWindowSize().X - SidebarWidth - GridSize)
			{
				// Pick up the palette selection.
				vector<Category> CategorisedNodes = Nodes.GetCategorisedNodes(); // todo: constantly calling this is terrible!
				const Category& CurrentCategory = CategorisedNodes[PaletteCategory];

				int PaletteSelection = GetPaletteSelection(CurrentCategory);

				if (PaletteSelection != -1)
				{
					NodeOnMouse = CurrentCategory.Nodes[PaletteSelection]->Clone();
				}
				else
				{
					// If there is no palette selection, look for the mouse being over a category button
					if (Event.motion.x < GetWindowSize().X - SidebarWidth && Event.motion.x > GetWindowSize().X - SidebarWidth - GridSize)
					{
						int Category = (Event.motion.y - ToolbarHeight - (ToolbarPadding * 2)) / GridSize;

						if (Category < CategorisedNodes.size())
						{
							PaletteCategory = Category;
						}
					}
				}

				return true; // always stop the event here
			}

			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			if (Event.button.button == 0 && Event.motion.x > GetWindowSize().X - SidebarWidth - GridSize)
			{
				if (NodeOnMouse)
				{
					CurrentFunction->RemoveNode(NodeOnMouse);
					NodeOnMouse.reset();
				}

				return true;
			}
			
			break;
		}
	}
	
	return false;
}

bool Alchemist::GridHandleEvent(SDL_Event& Event)
{
	switch(Event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
		{
			Point EventMousePosition(Event.motion.x, Event.motion.y);

			Point MouseGridPosition = ScreenToGrid(EventMousePosition);

			if (Event.button.button == 2)
			{
				ViewDrag = true;

				return true;
			}
			else if (Event.button.button == 1)
			{
				if (NodeBeingConnected)
				{
					if (NodeBeingConnectedTo)
					{
						// An options menu is open near this node.
						// Find if we just clicked any option.
						// The position of the menu options is fairly easy to work out.
						Point OptionPosition = GetOptionsMenuPosition(NodeBeingConnectedTo);
						Size OptionSize = GetOptionsMenuOptionSize(NodeBeingConnectedTo);

						for (int i = 0; i < NodeBeingConnectedTo->GetNumArguments(); i++)
						{
							SDL_Rect OptionRect = {
								OptionPosition.X,
								OptionPosition.Y + (ConnectMenuOptionPadding / 2) + (OptionSize.Y * i),
								OptionSize.X,
								OptionSize.Y
							};

							if (EventMousePosition.IsInRectangle(OptionRect))
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

				NodeLastSelected.reset();

				// Look for a node under the mouse.
				// If there is one, begin dragging or copy it.
				NodeOnMouse = CurrentFunction->GetNodeAt(MouseGridPosition);

				if (NodeOnMouse && Copy)
				{
					NodeOnMouse = NodeOnMouse->Clone();
				}

				return true;
			}
			else if (Event.button.button == 3 && !NodeOnMouse)
			{
				NodeBeingConnectedTo.reset();

				// Look for a node under the mouse.
				// If there is one, start creating a connector from this node.
				NodeBeingConnected = CurrentFunction->GetNodeAt(MouseGridPosition);

				return true;
			}

			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			if (Event.button.button == 2)
			{
				ViewDrag = false;
				return true;
			}
			else if (Event.button.button == 1)
			{
				// If there is a node on the mouse, and the grid space below the mouse is clear, insert the node there.
				if (NodeOnMouse)
				{
					NodeLastSelected = NodeOnMouse;

					Point MouseGridPosition = ScreenToGrid(Point(Event.motion.x, Event.motion.y));
					CurrentFunction->PlaceNode(NodeOnMouse, MouseGridPosition);

					NodeOnMouse.reset();
					
					return true;
				}
			}
			else if (Event.button.button == 3)
			{
				// If there is a connector being dragged, connect it to the node it was dropped on. Always stop dragging at this point
				if (NodeBeingConnected)
				{
					// If we dropped on another node, bring up the second stage connecting menu!
					Point MouseGridPosition = ScreenToGrid(Point(Event.motion.x, Event.motion.y));
					NodeBeingConnectedTo = CurrentFunction->GetNodeAt(MouseGridPosition);

					if (!NodeBeingConnectedTo || NodeBeingConnectedTo == NodeBeingConnected || NodeBeingConnectedTo->GetNumArguments() == 0)
					{
						NodeBeingConnected.reset();
						NodeBeingConnectedTo.reset();
					}
				}
				
				return true;
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

			return true;
		}
		case SDL_KEYDOWN:
		{
			if (Event.key.keysym.sym == SDLK_F5)
			{
				Compile();
			}
			else if (Event.key.keysym.sym == SDLK_LCTRL)
			{
				Copy = true;
			}
			else if (Event.key.keysym.sym == SDLK_DELETE)
			{
				shared_ptr<Node> LastSelectedLock = NodeLastSelected.lock();

				if (LastSelectedLock)
				{
					CurrentFunction->RemoveNode(LastSelectedLock);
				}
			}
			else
			{
				shared_ptr<Node> LastSelectedLock = NodeLastSelected.lock();

				if (LastSelectedLock)
				{
					LastSelectedLock->HandleKeyPress(Event);
				}
			}

			return true;
		}
		case SDL_KEYUP:
		{
			if (Event.key.keysym.sym == SDLK_LCTRL)
			{
				Copy = false;
			}

			return true;
		}
		case SDL_TEXTINPUT:
		{
			shared_ptr<Node> LastSelectedLock = NodeLastSelected.lock();

			if (LastSelectedLock)
			{
				LastSelectedLock->HandleTextInput(Event);
			}

			return true;
		}
	}

	return false;
}

int Alchemist::GetPaletteSelection(const Category& CurrentCategory) const
{
	int PaletteSelection = -1;

	for (int i = 0; i < CurrentCategory.Nodes.size(); i++)
	{
		Point NodePos = GetPaletteItemPosition(i);
		if (MousePos.X > NodePos.X && MousePos.Y > NodePos.Y && MousePos.X <= NodePos.X + GridSize && MousePos.Y <= NodePos.Y + GridSize)
		{
			PaletteSelection = i;
		}
	}

	return PaletteSelection;
}

void Alchemist::DrawConnectorArrowOnGrid(const Point& Point1, const Point& Point2, int ConnectorId) const
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

	// Draw number of connector
	if(ConnectorId != -1)
	{
		shared_ptr<Resource_Font> Font = GetDefaultFont();

		string IdStr = to_string(ConnectorId+1);

		SDL_Texture* DisplayTexture = Font->GetStringTexture(IdStr, 18);
		::Size DisplaySize = Font->GetStringScreenSize(IdStr, 18);

		SDL_Rect DisplayRect;

		DisplayRect.x = ScreenPoint2.X;
		DisplayRect.y = ScreenPoint2.Y + 20;
		DisplayRect.w = DisplaySize.X;
		DisplayRect.h = DisplaySize.Y;

		if (DisplayRect.x + DisplayRect.w > GetWindowSize().X)
		{
			DisplayRect.x -= DisplayRect.w;
		}

		SDL_SetTextureColorMod(DisplayTexture, 0, 0, 0);
		SDL_RenderCopy(Renderer, DisplayTexture, NULL, &DisplayRect);
	}
}

void Alchemist::DrawTooltip(const string& Text, int YOffset, int Size) const
{
	shared_ptr<Resource_Font> Font = GetDefaultFont();

	SDL_Texture* DisplayTexture = Font->GetStringTexture(Text, Size);
	::Size DisplaySize = Font->GetStringScreenSize(Text, Size);

	SDL_Rect DisplayRect;

	DisplayRect.x = MousePos.X + 20;
	DisplayRect.y = MousePos.Y + 20 + YOffset;
	DisplayRect.w = DisplaySize.X;
	DisplayRect.h = DisplaySize.Y;

	if (DisplayRect.x + DisplayRect.w > GetWindowSize().X)
	{
		DisplayRect.x -= DisplayRect.w;
	}

	SDL_SetTextureColorMod(DisplayTexture, 0, 0, 0);
	SDL_RenderCopy(Renderer, DisplayTexture, NULL, &DisplayRect);
}

void Alchemist::DrawNodeTooltip(const shared_ptr<Node>& NodeIn) const
{
	string NodeDetailText = NodeIn->GetDisplayName() + ":" + to_string(NodeIn->GetNumArguments());

	DrawTooltip(NodeDetailText);

	for(int i = 0; i < NodeIn->GetNumArguments(); i++)
	{
		if(shared_ptr<Node> Connector = NodeIn->GetConnector(i))
		{
			string DetailText = NodeIn->GetArgumentName(i) + " = " + Connector->GetDisplayName() + ":" + to_string(Connector->GetNumArguments());
			DrawTooltip(DetailText, 30 + (i * 20), 18);
		}
	}
}

shared_ptr<Resource_Font> Alchemist::GetDefaultFont() const
{
	return Resources.GetResource<Resource_Font>("Font.ttf");
}

vector<ToolbarOptionData> Alchemist::GetToolbarOptions() const
{
	vector<ToolbarOptionData> Out;
	
	// First option is the "[name of the function]:[arity]", example: main:0
	// Contains these options AS LONG AS the function name is NOT "Main":
	// - Change signature
	// - Delete
	{
		string FuncString = "Function (" + CurrentFunction->GetName() + ":" + to_string(CurrentFunction->GetArity()) + ")...";
		
		if (CurrentFunction->GetName() != "Main")
		{
			ToolbarOptionData Options = {
				FuncString,
				{ "Change Signature...", "Delete" },
				{
					[](Alchemist* Instance)
					{
						Instance->EditFunctionSignature();
					},
					[](Alchemist* Instance)
					{
						Instance->GetCurrentModule()->RemoveFunction(Instance->GetCurrentFunction()->GetName());
						Instance->ViewFunction("Main");
					}
				}
			};

			Out.push_back(Options);
		}
		else
		{
			ToolbarOptionData Options = {
				FuncString,
				{ "You cannot add arguments to or delete the \"main\" function." },
				{ nullptr }
			};

			Out.push_back(Options);
		}
	}

	// Second option is "Program"
	// Contains:
	// - New function
	// - Generate code
	{
		ToolbarOptionData Options = {
			"Program...",
			{ "New Function" },
			{
				[](Alchemist* Instance)
				{
					shared_ptr<Function> Func = Instance->GetCurrentModule()->CreateUniqueFunction();
					Instance->ViewFunction(Func->GetName());
				}
			}
		};

		Out.push_back(Options);
	}

	// Third option is "View"
	// Contains every function signature in the program.
	{
		ToolbarOptionData Options = {
			"View...",
			{},
			{}
		};

		auto Functions = CurrentModule.GetFunctions();
		
		for(int i = 0; i < Functions.size(); i++)
		{
			string FuncName = Functions[i]->GetName(); // needed due to quirks with capturing
			
			Options.SubOptions.push_back(Functions[i]->GetName() + ":" + to_string(Functions[i]->GetArity()));
			Options.SelectFunctions.push_back([FuncName](Alchemist* Instance)
			{
				Instance->ViewFunction(FuncName);
			});
		}

		Out.push_back(Options);
	}
	
	// Forth option is "About"
	// - Non-clickable version number
	// - Copyright Chris Sixsmith 2020.
	{
		ToolbarOptionData Options = {
			"About...",
			{ "Version 1", "Copyright Chris Sixsmith 2020." },
			{ nullptr, nullptr }
		};

		Out.push_back(Options);
	}
	
	return Out;
}

int Alchemist::GetToolbarOptionX(int OptionId) const
{
	int X = ToolbarPadding + 64;
	auto Options = GetToolbarOptions();
	shared_ptr<Resource_Font> Font = GetDefaultFont();
	
	for(int i = 0; i < OptionId && i < Options.size(); i++)
	{
		X += Font->GetStringScreenSize(Options[i].Option).X + (ToolbarPadding * 4);
	}
	
	return X;
}

SDL_Rect Alchemist::GetToolbarOptionSelectionRect(int OptionId) const
{
	auto Options = GetToolbarOptions();
	
	shared_ptr<Resource_Font> Font = GetDefaultFont();
	
	return {
		GetToolbarOptionX(OptionId),
		0,
		Font->GetStringScreenSize(Options[OptionId].Option).X + (ToolbarPadding * 4),
		ToolbarHeight + (ToolbarPadding * 2)
	};
}

SDL_Rect Alchemist::GetToolbarSubOptionRect(int OptionId, int SubOptionId) const
{
	int SubOptionMenuWidth = 0;
	auto Options = GetToolbarOptions();

	shared_ptr<Resource_Font> Font = GetDefaultFont();
	
	for(int i = 0; i < Options[OptionId].SubOptions.size(); i++)
	{
		int W = Font->GetStringScreenSize(Options[OptionId].SubOptions[i]).X;
		
		if(SubOptionMenuWidth < W)
		{
			SubOptionMenuWidth = W;
		}
	}
	
	return {
		GetToolbarOptionX(OptionId),
		ToolbarHeight + (ToolbarPadding * 2) + (ToolbarOptionHeight * SubOptionId),
		SubOptionMenuWidth + (ToolbarPadding * 2),
		ToolbarOptionHeight
	};
}
