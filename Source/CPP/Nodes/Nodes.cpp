// Copyright Chris Sixsmith 2020.

#include "Nodes.h"

#include <cassert>

Node::Node()
{
	// Nothing to do here!
	// Use the node constructor to register arguments.
}

VarType Node::GetArgumentType(int Argument) const
{
	assert(Argument > 0 && Argument < ArgumentData.size());
	return ArgumentData[Argument].ArgumentType;
}

string Node::GetArgumentName(int Argument) const
{
	assert(Argument > 0 && Argument < ArgumentData.size());
	return ArgumentData[Argument].ArgumentName;
}

int Node::GetArgumentIndexFromName(const string& ArgumentName) const
{
	auto Iterator = ArgumentLookupTable.find(ArgumentName);

	if(Iterator != ArgumentLookupTable.end())
	{
		return Iterator->second;
	}

	return -1;
}

Node* Node::GetConnector(int Argument) const
{
	assert(Argument > 0 && Argument < ArgumentData.size());
	return ArgumentData[Argument].Connector;
}

bool Node::SetConnector(Node* From, int Argument)
{
	// Still counts as an assertion failure if we provide an invalid index
	assert(Argument > 0 && Argument < ArgumentData.size());

	// TODO check type match
	
	// Success!
	ArgumentData[Argument].Connector = From;
	return true;
}

void Node::DisconnectConnector(int Argument)
{
	assert(Argument > 0 && Argument < ArgumentData.size());
	ArgumentData[Argument].Connector = nullptr;
}

void Node::RegisterArgument(VarType Type, const string& ArgumentName)
{
	assert(GetArgumentIndexFromName(ArgumentName) == -1);
	ArgumentData.push_back(NodeArgumentData{ Type, ArgumentName, nullptr });
}

void Node::Draw(SDL_Renderer* Renderer, Point Position) const
{
	// For default node drawing code, draw a circle.
	
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

vector<Node*> StaticNodes;

NodeManager::NodeManager()
{
}

NodeManager::~NodeManager()
{
}

Node* NodeManager::CreateNode(int NodeID) const
{
	return Get(NodeID)->Clone();
}

Node* NodeManager::Get(int NodeID) const
{
	if (NodeID >= 0)
	{
		// Return from the static nodes list.
		assert(NodeID < StaticNodes.size());
		return StaticNodes[NodeID];
	}
	else
	{
		// TODO return user-created function call node

	}

	return nullptr;
}

vector<Node*> NodeManager::GetAll() const
{
	// TODO account for user-created
	return StaticNodes;
}

vector<Node*> NodeManager::GetAll(string Category) const
{
	vector<Node*> All = GetAll();

	vector<Node*> Out;

	for(Node* Node : All)
	{
		if(Node->GetCategory() == Category)
		{
			Out.push_back(Node);
		}
	}

	return Out;
}

vector<string> NodeManager::GetCategories() const
{
	vector<Node*> All = GetAll();

	vector<string> Out;

	for (Node* Node : All)
	{
		string Category = Node->GetCategory();

		if(find(Out.begin(), Out.end(), Category) == Out.end())
		{
			Out.push_back(Category);
		}
	}

	return Out;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


NodeRegistrar::NodeRegistrar(Node* NodeType)
{
	NodeType->ID = StaticNodes.size();
	StaticNodes.push_back(NodeType);
}
