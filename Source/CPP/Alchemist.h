// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"
#include "2DPositioning.h"
#include "Nodes/Nodes.h"
#include "Resources/Resources.h"
#include "Module/Module.h"
#include "Resources/Resource_Font.h"

const int GridSize = 64;
const int SidebarWidth = 400;
const int SidebarItemCount = 4;
const int SidebarPadding = (SidebarWidth - (SidebarItemCount * GridSize)) / 8;
const int ConnectMenuOptionPadding = 16;

class Function;

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

	/** Converts screen position to graph position. */
	Point ScreenToGraph(const Point& ScreenPosition) const;

	/** Converts graph position to screen position. */
	Point GraphToScreen(const Point& GraphPosition) const;

	/** Converts graph position to grid position. */
	Point GraphToGrid(const Point& GraphPosition) const;

	/** Converts grid position to graph position. */
	Point GridToGraph(const Point& GridPosition) const;

	/** Converts grid position to screen position. */
	Point GridToScreen(const Point& GridPosition) const;

	/** Converts screen position to grid position. */
	Point ScreenToGrid(const Point& ScreenPosition) const;

	/** Returns screen position of palette item, taking into account current Palette Scroll. */
	Point GetPaletteItemPosition(int Index) const;

	/** Returns screen position of palette item, taking into account current Palette Scroll. */
	Point GetCategoryButtonPosition(int Index) const;

	/** Returns the renderer */
	SDL_Renderer* GetRenderer() { return Renderer; }

	/** Returns the node manager */
	NodeManager* GetNodeManager() { return &Nodes; }

	/** Returns the resource manager */
	ResourceManager* GetResourceManager() { return &Resources; }

	/** Returns the function being edited. */
	shared_ptr<Function> GetCurrentFunction() { return CurrentFunction; }

	/** Returns the module. */
	Module* GetCurrentModule() { return &CurrentModule; }
	
	/** Returns window size. */
	Size GetWindowSize() const;

	/** Returns size of options in the options menu rect spawned from given node. */
	Size GetOptionsMenuPosition(const shared_ptr<Node>& NodeOnGrid) const;
	
	/** Returns size of options in the options menu rect spawned from given node. */
	Size GetOptionsMenuOptionSize(const shared_ptr<Node>& NodeOnGrid, const shared_ptr<Resource_Font>& FontResource) const;

private:	
	/** Gets window start size. */
	Size GetWindowStartSize() const;

	/** Draws connector relative to grid. */
	void DrawConnectorArrowOnGrid(const Point& Point1, const Point& Point2);
	
private:
	bool Close = false;
	bool ViewDrag = false;

	int PaletteScroll = 0;
	int PaletteCategory = 0;

	Point ViewTopLeft;
	Point MousePos;
	
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	SDL_Surface* WindowSurface;

	NodeManager Nodes;
	ResourceManager Resources;

	shared_ptr<Node> NodeOnMouse;

	shared_ptr<Node> NodeBeingConnected;
	shared_ptr<Node> NodeBeingConnectedTo;

	Module CurrentModule;
	shared_ptr<Function> CurrentFunction;
};
