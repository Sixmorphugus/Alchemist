// Copyright Chris Sixsmith 2020.

#include "Nodes.h"
#include "DrawShapes.h"
#include "Alchemist.h"

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

shared_ptr<Node> Node::GetConnector(int Argument) const
{
	assert(Argument > 0 && Argument < ArgumentData.size());
	return ArgumentData[Argument].Connector;
}

bool Node::SetConnector(const shared_ptr<Node>& From, int Argument)
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

void Node::Draw(Alchemist* Instance, Point Position) const
{
	// For default node drawing code, draw a circle.
	
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


vector<shared_ptr<Node>>& GetStaticNodes()
{
	static vector<shared_ptr<Node>> StaticNodes;
	return StaticNodes;
}

NodeManager::NodeManager()
{
}

NodeManager::~NodeManager()
{
}

shared_ptr<Node> NodeManager::CreateNode(int NodeID) const
{
	return Get(NodeID)->Clone();
}

shared_ptr<Node> NodeManager::Get(int NodeID) const
{
	if (NodeID >= 0)
	{
		// Return from the static nodes list.
		assert(NodeID < GetStaticNodes().size());
		return GetStaticNodes()[NodeID];
	}
	else
	{
		// TODO return user-created function call node

	}

	return nullptr;
}

vector<shared_ptr<Node>> NodeManager::GetAll() const
{
	// TODO account for user-created
	return GetStaticNodes();
}

vector<shared_ptr<Node>> NodeManager::GetAll(string Category) const
{
	vector<shared_ptr<Node>> All = GetAll();

	vector<shared_ptr<Node>> Out;

	for(const shared_ptr<Node>& NodeInstance : All)
	{
		if(NodeInstance->GetCategory() == Category)
		{
			Out.push_back(NodeInstance);
		}
	}

	return Out;
}

vector<string> NodeManager::GetCategories() const
{
	vector<shared_ptr<Node>> All = GetAll();

	vector<string> Out;

	for (shared_ptr<Node> NodeInstance : All)
	{
		string Category = NodeInstance->GetCategory();

		if(find(Out.begin(), Out.end(), Category) == Out.end())
		{
			Out.push_back(Category);
		}
	}

	return Out;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


NodeRegistrar::NodeRegistrar(const shared_ptr<Node>& NodeType)
{
	NodeType->ID = (int)GetStaticNodes().size();
	GetStaticNodes().push_back(NodeType);
}
