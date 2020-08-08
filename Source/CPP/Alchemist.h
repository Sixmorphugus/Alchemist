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
const int ToolbarPadding = 4;
const int ToolbarHeight = 40;
const int ToolbarOptionHeight = 40;

struct ToolbarOptionData
{
	string Option;
	vector<string> SubOptions;
	vector<function<void(Alchemist* Instance)>> SelectFunctions;
};

class Function;

class Alchemist
{
public:
	/** Alchemist's constructor initializes an instance of the environment. */
	Alchemist();

	/** Alchemist's destructor shuts down an instance of the environment. */
	~Alchemist();

	/** Will run the environment until the user closes it. */
	void Run();

	/** Will attempt to compile the program the user has created. */
	void Compile();
	
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
	SDL_Renderer* GetRenderer() const { return Renderer; }

	/** Returns the node manager */
	NodeManager* GetNodeManager() { return &Nodes; }
	const NodeManager* GetNodeManager() const { return &Nodes; }

	/** Returns the resource manager */
	ResourceManager* GetResourceManager() { return &Resources; }
	const ResourceManager* GetResourceManager() const { return &Resources; }
	
	/** Returns the function being edited. */
	shared_ptr<Function> GetCurrentFunction() { return CurrentFunction; }

	/** Returns the module. */
	Module* GetCurrentModule() { return &CurrentModule; }
	
	/** Returns window size. */
	Size GetWindowSize() const;

	/** Returns size of options in the options menu rect spawned from given node. */
	Size GetOptionsMenuPosition(const shared_ptr<Node>& NodeOnGrid) const;
	
	/** Returns size of options in the options menu rect spawned from given node. */
	Size GetOptionsMenuOptionSize(const shared_ptr<Node>& NodeOnGrid) const;

	/** Views a function */
	void ViewFunction(string Name);

	/** Edits function signature. */
	void EditFunctionSignature();

private:	
	/** Gets window start size. */
	Size GetWindowStartSize() const;

	/** Draws the toolbar. */
	void DrawToolbar() const;
	
	/** Draws the node palette. */
	void DrawNodePalette() const;
	
	/** Draws the grid. */
	void DrawGrid() const;

	/** Tries to make the toolbar handle an event. */
	bool ToolbarHandleEvent(SDL_Event& Event);
	
	/** Tries to make the palette handle an event. */
	bool PaletteHandleEvent(SDL_Event& Event);
	
	/** Tries to make the grid handle an event. */
	bool GridHandleEvent(SDL_Event& Event);

	/** Gets palette selection. */
	int GetPaletteSelection(const Category& CurrentCategory) const;
	
	/** Draws connector relative to grid. */
	void DrawConnectorArrowOnGrid(const Point& Point1, const Point& Point2) const;

	/** Draws tooltip. */
	void DrawTooltip(const shared_ptr<Node>& node) const;

	/** Returns the default font. */
	shared_ptr<Resource_Font> GetDefaultFont() const;

	/** Returns the current list of toolbar options. */
	vector<ToolbarOptionData> GetToolbarOptions() const;

	/** Calculates toolbar option X position. */
	int GetToolbarOptionX(int OptionId) const;

	/** Calculates toolbar option rectangle. */
	SDL_Rect GetToolbarOptionSelectionRect(int OptionId) const;

	/** Calculates toolbar suboption rectangle. */
	SDL_Rect GetToolbarSubOptionRect(int OptionId, int SubOptionId) const;
	
private:
	bool Close = false;
	bool ViewDrag = false;
	bool Copy = false;

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

	weak_ptr<Node> NodeLastSelected;

	Module CurrentModule;
	shared_ptr<Function> CurrentFunction;

	int ToolbarOpenMenu = -1;

	bool EditingFunctionSignature = false;

	vector<CompilationProblem> ProblemsFromLastCompile;
};
