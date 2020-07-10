// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"

class Node;

/**
 * Enum representing all the variable types programs created in Alchemist support.
 * Some are conglomerations.
 */
enum class VarType
{
	Invalid = 0,
	Any,

	// Numbers
	Integer,
	Float,

	// Data
	Atom,
	String, // NTS an Erlang String is just a List pretending to be a String - it is its own type in Alchemist
	Binary, // We do not support bit string sizes that are not divisable by 8

	// Container
	Tuple,
	Map,
	List,
};


/** Node argument data. */
struct NodeArgumentData
{	
	/** The argument's type. */
	VarType ArgumentType = VarType::Invalid;

	/** The argument's name. */
	string ArgumentName;

	/** Node linked from. */
	Node* Connector = nullptr;
};
