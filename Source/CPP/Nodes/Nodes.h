// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"

// Reserved IDs
#define NODE_ID_ROOT 0
#define NODE_ID_CONSTANT_REF 1
#define NODE_ID_FUNCTION_REF 2
#define NODE_ID_FUNCTION_CALL 3
#define NODE_ID_FIRST_REGISTERED 4

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
 * - Asking this to create from a negative ID will create a user function call node that calls the given positive ID.
 */
class NodeManager
{
public:
	
};
