// Copyright Chris Sixsmith 2020.

#include "Nodes.h"
#include "DrawShapes.h"
#include "Alchemist.h"
#include "Resources/Resource_Image.h"
#include "Special/Node_UserDefined.h"

Node::Node()
	: ID(-1)
{
	// Nothing to do here!
	// Use the node constructor to register arguments.
}

bool Node::GetArgumentIsPattern(int Argument) const
{
	assert(Argument >= 0 && Argument < ArgumentData.size());
	return ArgumentData[Argument].IsPattern;
}

string Node::GetArgumentName(int Argument) const
{
	assert(Argument >= 0 && Argument < ArgumentData.size());
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
	assert(Argument >= 0 && Argument < ArgumentData.size());
	return ArgumentData[Argument].Connector.lock();
}

bool Node::SetConnector(const shared_ptr<Node>& From, int Argument)
{
	// Still counts as an assertion failure if we provide an invalid index
	assert(Argument >= 0 && Argument < ArgumentData.size());

	// TODO check type match
	
	// Success!
	ArgumentData[Argument].Connector = From;
	return true;
}

void Node::DisconnectConnector(int Argument)
{
	assert(Argument >= 0 && Argument < ArgumentData.size());
	ArgumentData[Argument].Connector.reset();
}

void Node::RegisterArgument(const string& ArgumentName, bool IsPattern)
{
	assert(GetArgumentIndexFromName(ArgumentName) == -1);
	ArgumentData.push_back(NodeArgumentData{ ArgumentName, IsPattern, weak_ptr<Node>() });
}

void Node::ClearArguments()
{
	ArgumentData.clear();
	ArgumentLookupTable.clear();
}

void Node::Draw(const Alchemist* Instance, const Point& Position, bool IsPreview) const
{
	shared_ptr<Resource_Image> RingResource = Instance->GetResourceManager()->GetResource<Resource_Image>("NodeRing.png");
	shared_ptr<Resource_Image> NodeResource = Instance->GetResourceManager()->GetResource<Resource_Image>("Node.png");

	assert(RingResource);
	assert(NodeResource);

	SDL_SetTextureAlphaMod(RingResource->GetTexture(), IsPreview ? 150 : 255);
	SDL_SetTextureAlphaMod(NodeResource->GetTexture(), IsPreview ? 150 : 255);

	SDL_Rect Rect = GetRenderRect(Position);
	
	SDL_RenderCopy(Instance->GetRenderer(), RingResource->GetTexture(), NULL, &Rect);
	SDL_RenderCopy(Instance->GetRenderer(), NodeResource->GetTexture(), NULL, &Rect);
}

SDL_Rect Node::GetRenderRect(const Point& Position) const
{
	return SDL_Rect{Position.X, Position.Y, GridSize, GridSize};
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


vector<shared_ptr<Node>>& GetStaticNodes()
{
	static vector<shared_ptr<Node>> StaticNodes;
	return StaticNodes;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


NodeManager::NodeManager(Alchemist* InstanceIn)
	: Instance(InstanceIn)
{}

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
		return GetAllUser()[-NodeID];
	}

	return nullptr;
}

vector<shared_ptr<Node>> NodeManager::GetAll() const
{
	vector<shared_ptr<Node>> Nodes = GetAllStatic();
	vector<shared_ptr<Node>> UserNodes = GetAllUser();

	Nodes.insert(Nodes.end(), UserNodes.begin(), UserNodes.end());
	
	return Nodes;
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

vector<shared_ptr<Node>> NodeManager::GetAllStatic() const
{
	return GetStaticNodes();
}

vector<shared_ptr<Node>> NodeManager::GetAllUser() const
{
	vector<shared_ptr<Node>> Out;
	auto Functions = Instance->GetCurrentModule()->GetFunctions();
	
	for(int i = 0; i < Functions.size(); i++)
	{
		shared_ptr<Node> UserNode = make_shared<Node_UserDefined>(Functions[i]);
		UserNode->ID = -i;
		
		Out.push_back(UserNode);
	}

	return Out;
}

vector<Category> NodeManager::GetCategorisedNodes() const
{
	vector<shared_ptr<Node>> All = GetAll();

	vector<Category> Out;
	unordered_map<string, int> CategoryLookup;

	for (shared_ptr<Node> NodeInstance : All)
	{
		// Find category ID
		string CategoryName = NodeInstance->GetCategory();
		
		if (CategoryName == "")
		{
			continue;
		}

		auto FoundCategory = CategoryLookup.find(CategoryName);
		int FoundCategoryId;

		if (FoundCategory == CategoryLookup.end())
		{
			// Add a new category.
			Out.push_back(Category{ CategoryName, {} });

			FoundCategoryId = (int)Out.size() - 1;

			CategoryLookup[CategoryName] = FoundCategoryId;			
		}
		else
		{
			FoundCategoryId = FoundCategory->second;
		}

		// Add node to category
		Out[FoundCategoryId].Nodes.push_back(NodeInstance);
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
