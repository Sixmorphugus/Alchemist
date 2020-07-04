// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"

#define NODE_ID_ROOT 0
#define NODE_ID_FIRST_REGISTERED 1
#define NODE_ID_FIRST_USER_DEFINED -1

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
};

/**
 * Node manager class.
 * Create one of these and use it to get and edit the list of known nodes for a project.
 * The node manager does not know about special types of node like the root node. However, it acts as the provider for all other nodes.
 * - All node types registered at compile time are given an ID
 * - New nodes are created by "cloning" a template (all nodes have a Clone function which returns a Node*)
 * - Negative IDs are user created nodes
 * - All nodes are double categorised with the category they are registered with and you can filter by category and subcategory (this will iterate the whole node list, as there is no acceleration for it yet)
 * - You can edit the negative (user-created) node list whenever you want but not the hard coded (auto-registered) nodes with positive IDs.
 * - Node ID zero (0) is reserved for root nodes, meaning each node can be described by an ID in files. IDs 1 onwards are currently hardcoded nodes but this may change.
 */
class NodeManager
{
public:
	
};
