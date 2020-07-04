// Copyright Chris Sixsmith 2020.

#pragma once

/**
 * Enum representing all the variable types programs created in Alchemist support.
 * Some are conglomerations.
 */
enum class VarType
{
	Invalid = 0,

	// Numbers
	Integer,
	Float,

	// Data
	Atom,
	String, // NTS an Erlang String is just a List pretending to be a String - it is its own type in Alchemist
	Binary, // We do not support bit string sizes that are not dividable by 8

	// Container
	Tuple,
	Map,
	List,
};