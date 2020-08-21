// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"
#include "Variables.h"
#include "2DPositioning.h"
#include "CompilationProblem.h"

class Alchemist;
class Function;

// todo adapt to shared_ptr (i.e. stop using dumb ptr)

/**
 * Node class.
 * Provides:
 * - Constructor to register it with
 * - Clone function
 * - Serialize and Deserialize functions for saving
 * - Type node returns
 * - Argument names and types
 * - EmitInternal function for creating a line of Erlang code.
 */
class Node : public enable_shared_from_this<Node>
{
public:
	Node();

	/** Makes a copy of this node. */
	virtual shared_ptr<Node> Clone() const = 0;

	friend class NodeManager;
	friend class NodeRegistrar;
	friend class Function;


	// Metadata.
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////


public:
	/** Returns node's display name. This is what you see as a tooltip when you mouse over the node. */
	virtual string GetDisplayName() const = 0;

	/** Returns node's category. If this returns "", the node will be hidden from all menus. */
	virtual string GetCategory() const { return ""; }


	// Serialisation. Note that most nodes have no special data.
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////


public:
	/** Loads the node's data from a file. */
	virtual void Load(const ifstream& FileStream) {}

	/** Saves this node's data to a file. */
	virtual void Save(const ofstream& FileStream) const {}

	/** Returns the size this node will use for its serialised data packet in an .ALCH file. */
	virtual size_t GetDataSize() const { return 0; }


	// Variable Data.
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////


public:
	/** Returns this node's argument count. */
	int GetNumArguments() const { return (int)ArgumentData.size(); }

	/** Returns given argument's argument type. */
	bool GetArgumentIsPattern(int Argument) const;

	/** Returns given argument's argument name. */
	string GetArgumentName(int Argument) const;

	/**
	 * Returns given argument's argument index.
	 * Returns -1 if no such argument exists.
	 */
	int GetArgumentIndexFromName(const string& ArgumentName) const;

public:
	/** Returns given argument's connector. */
	shared_ptr<Node> GetConnector(int Argument) const;

	/** Sets given argument's connector, if allowed. */
	bool SetConnector(const shared_ptr<Node>& From, int Argument);

	/** Destroys a connector. */
	void DisconnectConnector(int Argument);

protected:
	/**
	 * Registers an argument. Call this in your node class's constructor.
	 * 2 arguments can't have the same name!
	 */
	void RegisterArgument(const string& ArgumentName, bool IsPattern = false);

	/** Removes all arguments. */
	void ClearArguments();

private:
	vector<NodeArgumentData> ArgumentData;
	unordered_map<string, int> ArgumentLookupTable;


// Misc.
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


public:
	/** Checks for an infinite loop before calling EmitInternal. */
	bool Emit(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path);
	
	/** Draws the node somewhere on-screen. */
	virtual void Draw(const Alchemist* Instance, const Point& Position, bool IsPreview = false) const;

	/** Handles unhandled key presses. */
	virtual void HandleKeyPress(const SDL_Event& Event) {}
	
	/** Handles text input. */
	virtual void HandleTextInput(const SDL_Event& Event) {}
	
	/** Gets the screen rectangle where this node is drawn given a draw position. */
	virtual SDL_Rect GetRenderRect(const Point& Position) const;

	/** Returns the node's ID to the NodeManager that manages it. */
	int GetID() const { return ID; }

	/** Returns the function the node is inside. */
	Function* GetFunction() const { return NodeFunction; }

	/** Return's the node's cached grid position. */
	Point GetGridPosition() const { return GridPosition; }

protected:
	/** Emits this node's Erlang code. Arguably the most important function. */
	virtual bool EmitInternal(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path) = 0;
	
	/** Triggers when an instance of the node is placed in a function, or the function signature is changed. */
	virtual void OnFunctionChanged() {}

	/** Triggers when an instance of the node is present in a function while the makeup of the containing Module is changed. */
	virtual void OnModuleChanged() {}
	
private:
	int ID = -1;
	Function* NodeFunction;
	Point GridPosition;

	friend class Module;
	
	// TODO Erlang code emit
};


/** NodeManager category. */
struct Category
{
	string Name;
	vector<shared_ptr<Node>> Nodes;
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
	NodeManager(Alchemist* InstanceIn);

	// Non copyable!
	NodeManager(const NodeManager&);
	NodeManager& operator=(const NodeManager&);

	/** Creates a node with the given ID. Remember, positive IDs give the built-in nodes, negative ones give user-created ones. */
	shared_ptr<Node> CreateNode(int NodeID) const;

	/** Finds the node matching the given class and creates it. */
	template<class NodeClass>
	shared_ptr<NodeClass> CreateNode() const
	{
		return dynamic_pointer_cast<NodeClass>(Get<NodeClass>()->Clone());
	}

	/** Returns the default object for a node. Don't let the user place this one! */
	shared_ptr<Node> Get(int NodeID) const;

	/** Returns the default object for a node given a class. Don't let the user place this one! */
	template<class NodeClass>
	shared_ptr<NodeClass> Get() const
	{
		vector<shared_ptr<Node>> AllNodes = GetAll();

		for(const shared_ptr<Node>& NodeInstance : AllNodes)
		{
			return dynamic_pointer_cast<NodeClass>(NodeInstance);
		}

		return nullptr;
	}

	/** Returns a vector of every node registered. */
	vector<shared_ptr<Node>> GetAll() const;

	/**
	 * Returns a vector of every node, filtered to a specific category.
	 * Note that this is slow as it isn't using an acceleration structure yet.
	 */
	vector<shared_ptr<Node>> GetAll(string Category) const;

	/** Returns a vector of every static node. */
	vector<shared_ptr<Node>> GetAllStatic() const;
	
	/** Returns a vector of every user node. */
	vector<shared_ptr<Node>> GetAllUser() const;

	/**
	 * Returns all categories that exist.
	 * Note that this is slow as it isn't using an acceleration structure yet.
	 */
	vector<Category> GetCategorisedNodes() const;
	
	// TODO user function registration.

private:
	Alchemist* Instance;
};


/**
 * Node Registrar.
 * Used to create a node at compile time via DECLARE_NODE().
 * Do not use directly!
 */
class NodeRegistrar
{
public:
	NodeRegistrar(const shared_ptr<Node>& NodeType);
};

#define DECLARE_NODE(NodeClass, ...) NodeRegistrar NodeClass ## Def (make_shared<NodeClass>(__VA_ARGS__))
#define DECLARE_NODE_CUSTOMNAME(Name, NodeClass, ...) NodeRegistrar Name (make_shared<NodeClass>(__VA_ARGS__))