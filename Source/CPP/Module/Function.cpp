#include "Function.h"

Function::Function(Alchemist* InstanceIn, string NameIn, int ArityIn)
	: Instance(InstanceIn), Name(NameIn), Arity(ArityIn)
{}

bool Function::PlaceNode(shared_ptr<Node> NewNode, const Point& Position)
{
	if (GetNodeAt(Position))
	{
		// Space is occupied already
		return false;
	}

	// Is this node on the grid already? (i.e. being moved)
	int Found = -1;

	for (int i = 0; i < NodesOnGrid.size(); i++)
	{
		if (NodesOnGrid[i] == NewNode)
		{
			// We need to remove the lookup table entries.
			Point PreviousPosition = GridReverseLookup[i];

			if (PreviousPosition == Position)
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
	if (Found == -1)
	{
		NodesOnGrid.push_back(NewNode);
		Found = (int)NodesOnGrid.size() - 1;
	}

	// Add to lookups
	GridLookup[Position] = Found;
	GridReverseLookup[Found] = Position;

	// Tell node it's in a function now
	if (NewNode->NodeFunction != this)
	{
		NewNode->NodeFunction = this;
		NewNode->OnPlaced();
	}

	NewNode->GridPosition = Position;

	return true;
}

shared_ptr<Node> Function::GetNodeAt(const Point& Position) const
{
	auto Found = GridLookup.find(Position);

	if (Found != GridLookup.end())
	{
		return GetNode(Found->second);
	}

	return nullptr;
}

shared_ptr<Node> Function::GetNode(int ID) const
{
	return NodesOnGrid[ID];
}

void Function::RemoveNode(int ID)
{
	// TODO
}