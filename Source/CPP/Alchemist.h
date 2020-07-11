// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"
#include "2DPositioning.h"
#include "Nodes/Nodes.h"
#include "Resources/Resources.h"

const int GridSize = 64;

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
	Point ScreenToGraph(const Point& ScreenPosition);

	/** Converts graph position to screen position. */
	Point GraphToScreen(const Point& GraphPosition);

	/** Converts graph position to grid position. */
	Point GraphToGrid(const Point& GraphPosition);

	/** Converts grid position to graph position. */
	Point GridToGraph(const Point& GridPosition);

	/** Converts grid position to screen position. */
	Point GridToScreen(const Point& GridPosition);

	/** Converts screen position to grid position. */
	Point ScreenToGrid(const Point& ScreenPosition);
	
	/** Creates a node at the given grid position. */
	Node* CreateNode(int NodeID, const Point& Position)
	{
		Node* NewNode = Nodes.CreateNode(NodeID);
		PlaceNode(NewNode, Position);

		return NewNode;
	}

	/** Finds the node matching the given class and creates it at the given grid position. */
	template<class NodeClass>
	Node* CreateNode(const Point& Position)
	{
		Node* NewNode = Nodes.CreateNode<NodeClass>();
		PlaceNode(NewNode, Position);

		return NewNode;
	}

	/** Places a given node at a given grid position. If it is already on the grid, it gets removed first. */
	void PlaceNode(Node* NewNode, const Point& Position);

	/** Returns the node at the specified grid position, if there is any. */
	Node* GetNodeAt(const Point& Position) const;

	/** Returns node with ID given. */
	Node* GetNode(int ID) const;

	/** Deletes node with ID given. */
	void RemoveNode(int ID);

	/** Returns the renderer */
	SDL_Renderer* GetRenderer() { return Renderer; }

	/** Returns the node manager */
	NodeManager* GetNodeManager() { return &Nodes; }

	/** Returns the resource manager */
	ResourceManager* GetResourceManager() { return &Resources; }
	
private:	
	/** Gets window start size. */
	Size _GetWindowStartSize();
	
private:
	bool Close = false;
	bool ViewDrag = false;

	Point ViewTopLeft;
	
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	SDL_Surface* WindowSurface;

	NodeManager Nodes;
	ResourceManager Resources;

	vector<Node*> NodesOnGrid; // nodes we manage
	unordered_map<Point, int> GridLookup; // for looking up nodes from grid positions
	unordered_map<int, Point> GridReverseLookup; // for looking up grid positions from nodes
};
