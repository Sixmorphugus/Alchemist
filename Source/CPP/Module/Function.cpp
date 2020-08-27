#include "Function.h"

#include "Nodes/Special/Node_Root.h"

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
		NewNode->OnFunctionChanged();
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
	if (GridReverseLookup.find(ID) != GridReverseLookup.end())
	{
		NodesOnGrid.erase(NodesOnGrid.begin() + ID);
		FixLookups();
	}	
}

void Function::RemoveNode(const shared_ptr<Node>& NodeInstance)
{
	int NodeId = GetNodeId(NodeInstance);

	if (NodeId != -1)
	{
		RemoveNode(NodeId);
	}
}

int Function::GetNodeId(const shared_ptr<Node>& NodeInstance) const
{
	for (int i = 0; i < (int)NodesOnGrid.size(); i++)
	{
		if (NodesOnGrid[i] == NodeInstance)
		{
			return i;
		}
	}

	return -1;
}

void Function::SetArity(int NewArity)
{
	Arity = NewArity;

	for(const shared_ptr<Node> Node : NodesOnGrid)
	{
		Node->OnFunctionChanged();
	}

	ParentModule->BroadcastOnModuleChanged();
}

bool Function::Emit(string& Output, vector<CompilationProblem>& Problems) const
{
	// First we need to find our list of root nodes.
	// These nodes determine the different function patterns.
	// The ordering is left->right
	vector<shared_ptr<Node_Root>> RootNodes = GetNodesOfClass<Node_Root>();

	// Sort nodes by their x coordinate (lower->higher)
	struct {
		bool operator()(const shared_ptr<Node>& LHS, const shared_ptr<Node>& RHS) const
		{
			return (LHS->GetGridPosition().X < RHS->GetGridPosition().X);
		}
	} SortNodes;
	
	sort(RootNodes.begin(), RootNodes.end(), SortNodes);
	
	// Once that is done, we build a script by, for each root node:
	// - Outputting a function header including the root node's argument pattern and, if one was defined, its guard sequence. 
	// - Going backwards up the tree from the RetVal and generating output expressions, assigning them to variables.
	// The generated code will end with the final expression in the tree (the one connected to RetVal) as the return value.

	bool Pass = true;
	
	for(int i = 0; i < RootNodes.size(); i++)
	{
		if(!RootNodes[i]->Emit(Output, Problems, {}))
		{
			Pass = false;
		}

		// If last root node, end with semicolon (;) - otherwise, with full stop (.)
		Output += i + 1 < RootNodes.size() ? ";" : ".";
		Output += "\n";
	}
	
	// Success?
	return Pass;
}

void Function::Rename(const string& NewName)
{
	if(GetInstance()->GetCurrentModule()->GetFunction(NewName))
	{
		return;
	}
	
	Name = NewName;
	Instance->GetCurrentModule()->UpdateLookups();
}

void Function::FixLookups()
{
	GridLookup.clear();
	GridReverseLookup.clear();
	
	for(int i = 0; i < (int)NodesOnGrid.size(); i++)
	{
		GridLookup[NodesOnGrid[i]->GetGridPosition()] = i;
		GridReverseLookup[i] = NodesOnGrid[i]->GetGridPosition();
	}
}
