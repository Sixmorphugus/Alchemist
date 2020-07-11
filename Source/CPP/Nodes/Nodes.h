// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"
#include "Variables.h"
#include "2DPositioning.h"

class Alchemist;

/**
 * Node class.
 * Provides:
 * - Constructor to register it with
 * - Clone function
 * - Serialize and Deserialize functions for saving
 * - Type node returns
 * - Argument names and types
 * - Emit function for creating a line of Erlang code.
 */
class Node
{
public:
	Node();
	
	/** Makes a copy of this node. */
	virtual Node* Clone() const = 0;

	friend class NodeManager;
	friend class NodeRegistrar;

	
// Metadata.
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


public:
	/** Returns node's display name. This is what you see as a tooltip when you mouse over the node. */
	virtual string GetDisplayName() const = 0;

	/** Returns node's category. If this returns "", the node will be hidden from all menus. */
	virtual string GetCategory() const { return ""; }

	/** Return's node's subcategory. If this returns "", the node will be hidden from all menus. */
	virtual string GetSubCategory() const { return ""; }
	
	
// Serialisation. Note that most nodes have no special data.
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
	

public:
	/** Loads the node's data from a file. */
	virtual void Load(const ifstream& FileStream) {}

	/** Saves this node's data to a file. */
	virtual void Save(const ofstream& FileStream) const {}

	/** Returns the size this node will use for its serialised data packet in an .ALCH file. */
	virtual size_t GetDataSize() { return 0; }


// Variable Data.
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


public:
	/** Gets return type for this node. */
	virtual VarType GetReturnType() const = 0;

	/** Returns this node's argument count. */
	size_t GetNumArguments() const { return ArgumentData.size(); }

	/** Returns given argument's argument type. */
	VarType GetArgumentType(int Argument) const;

	/** Returns given argument's argument name. */
	string GetArgumentName(int Argument) const;

	/**
	 * Returns given argument's argument index.
	 * Returns -1 if no such argument exists.
	 */
	int GetArgumentIndexFromName(const string& ArgumentName) const;

public:
	/** Returns given argument's connector. */
	Node* GetConnector(int Argument) const;

	/** Sets given argument's connector, if allowed. */
	bool SetConnector(Node* From, int Argument);

	/** Destroys a connector. */
	void DisconnectConnector(int Argument);

protected:
	/**
	 * Registers an argument. Call this in your node class's constructor.
	 * 2 arguments can't have the same name!
	 */
	void RegisterArgument(VarType Type, const string& ArgumentName);
	
private:
	vector<NodeArgumentData> ArgumentData;
	unordered_map<string, int> ArgumentLookupTable;
	

// Misc.
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


public:
	/** Draws the node somewhere on-screen. */
	void Draw(Alchemist* Instance, Point Position) const;

	/** Returns the node's ID to the NodeManager that manages it. */
	int GetID() { return ID; }
	
private:
	int ID;
	
	// TODO Erlang code emit
};


/**
 * Node manager class.
 * Create one of these and use it to get and edit the list of known nodes for a project.
 * The node manager does not know about special types of node like the root node. However, it acts as the provider for all other nodes.
 * - All node types registered at compile time are given an ID
 * - New nodes are created by "cloning" a template (all nodes have a Clone function which returns a Node*)
 * - Asking this to create from a negative ID will create a user function call node that calls the given positive ID.
 */
class NodeManager
{
public:
	NodeManager();
	~NodeManager();

	// Non copyable!
	NodeManager(const NodeManager&);
	NodeManager& operator=(const NodeManager&);

	/** Creates a node with the given ID. Remember, positive IDs give the built-in nodes, negative ones give user-created ones. */
	Node* CreateNode(int NodeID) const;

	/** Finds the node matching the given class and creates it. */
	template<class NodeClass>
	Node* CreateNode() const
	{
		return Get<NodeClass>()->Clone();
	}

	/** Returns the default object for a node. Don't let the user place this one! */
	Node* Get(int NodeID) const;

	/** Returns the default object for a node given a class. Don't let the user place this one! */
	template<class NodeClass>
	Node* Get() const
	{
		vector<Node*> AllNodes = GetAll();

		for(Node* Node : AllNodes)
		{
			NodeClass* NodeCasted = dynamic_cast<NodeClass*>(Node);

			if(NodeCasted)
			{
				return NodeCasted;
			}
		}

		return nullptr;
	}

	/** Returns a vector of every node registered. */
	vector<Node*> GetAll() const;

	/**
	 * Returns a vector of every node, filtered to a specific category.
	 * Note that this is slow as it isn't using an acceleration structure yet.
	 */
	vector<Node*> GetAll(string Category) const;

	/**
	 * Returns all categories that exist.
	 * Note that this is slow as it isn't using an acceleration structure yet.
	 */
	vector<string> GetCategories() const;
	
	// TODO user function registration.
};


/**
 * Node Registrar.
 * Used to create a node at compile time via DECLARE_NODE().
 * Do not use directly!
 */
class NodeRegistrar
{
public:
	NodeRegistrar(Node* NodeType);
};

#define DECLARE_NODE(NodeClassName, NodeDefinition) NodeRegistrar NodeClassName ## Def (new NodeDefinition);
