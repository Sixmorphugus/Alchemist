// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"
#include "Alchemist.h"

/**
 * A function within the user's program.
 */
class Function
{
public:
	Function(Alchemist* InstanceIn, string NameIn, int ArityIn);

	/** Creates a node at the given grid position. */
	shared_ptr<Node> CreateNode(int NodeID, const Point& Position)
	{
		shared_ptr<Node> NewNode = Instance->GetNodeManager()->CreateNode(NodeID);
		PlaceNode(NewNode, Position);

		return NewNode;
	}

	/** Finds the node matching the given class and creates it at the given grid position. */
	template<class NodeClass>
	shared_ptr<NodeClass> CreateNode(const Point& Position)
	{
		shared_ptr<NodeClass> NewNode = Instance->GetNodeManager()->CreateNode<NodeClass>();
		PlaceNode(NewNode, Position);

		return NewNode;
	}

	/**
	 * Places a given node at a given grid position. If it is already on the grid, it gets removed first.
	 * Returns true on success, false if something was in the way.
	 */
	bool PlaceNode(shared_ptr<Node> NewNode, const Point& Position);

	/** Returns the node at the specified grid position, if there is any. */
	shared_ptr<Node> GetNodeAt(const Point& Position) const;

	/** Returns node with ID given. */
	shared_ptr<Node> GetNode(int ID) const;

	/** Deletes node with ID given. */
	void RemoveNode(int ID);

	/** Deletes node. */
	void RemoveNode(const shared_ptr<Node>& NodeInstance);

	/** Deletes node. */
	int GetNodeId(const shared_ptr<Node>& NodeInstance) const;

	/** Returns a copy of the list of all nodes. */
	vector<shared_ptr<Node>> GetNodesOnGrid() const { return NodesOnGrid; }

	/** Returns function's arity (argument count). */
	int GetArity() const { return Arity; }

	/** Returns function's name. */
	string GetName() const { return Name; }

	/** Returns alchemist application instance. */
	Alchemist* GetInstance() const { return Instance; }

private:
	/** Recreates all values in the lookup table. */
	void FixLookups();
	
private:
	Alchemist* Instance;

	vector<shared_ptr<Node>> NodesOnGrid;
	unordered_map<Point, int> GridLookup; // for looking up nodes from grid positions
	unordered_map<int, Point> GridReverseLookup; // for looking up grid positions from nodes

	string Name;
	int Arity = 0;
	Module* ParentModule;

	friend class Module;
};