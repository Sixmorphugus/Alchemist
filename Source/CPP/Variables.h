// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"

class Node;


/**
 * Enum representing all the variable types programs created in Alchemist support.
 * Some are conglomerations.
 * All function arguments accept Any type. Built in functions expect specific types.
 */
enum class Type
{
	Invalid = 0,
	Any,

	// Numbers
	Integer,
	Float,

	// Data
	Atom,
	String, // An Erlang String is just a List pretending to be a String - it is its own type in Alchemist
	Binary, // We do not support bit string sizes that are not divisable by 8

	// Container
	Tuple,
	Map,
	List,
};


/** Node argument data. */
struct NodeArgumentData
{
	/** The argument's name. */
	string ArgumentName;

	/** Whether the argument expression is treated as a pattern (i.e. must be evaluatable at compile time, but is allowed to contain unassigned variables). */
	bool IsPattern = false;

	/** Node linked from. */
	weak_ptr<Node> Connector;
};
