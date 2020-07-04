// Copyright Chris Sixsmith 2020.

#pragma once

/**
 * Serialiser class.
 * Saving and loading an Alchemist project is fairly simple.
 * 
 * To Save, we:
 * - Open a file for binary writing.
 * 
 * - Write a header explaining not to mess with the file in ASCII text, followed by a new line; "### This is an Alchemist project data file. DO NOT mess with it! ###\n"
 * - Write "ALCH" in characters.
 * - Write the format version (int32).
 * 
 * - Write the constant count (int32).
 * - Write the size of the following list of constants (size_t).
 * - For each constant;
 *     - Write an index beginning at 0 (int32).
 *     - Write the name of the constant (size_t string length, followed by string bytes).
 *     - Write the constant's type (uint8).
 *     - Write the serialized data size for the constant's value (size_t).
 *     - Write the serialized data for the constant's value.
 * 
 * - Write the function count (int32).
 * - Write the size of the following list of function headers (size_t).
 * - We write the function header blocks first, since we need these to know what the user nodes are and their attributes. For each function;
 *     - Write a header block size (size_t).
 *     - Write the function's index (1 and above are valid indices, and will become negative for the function's ID to the Node Manager; int32).
 *     - Write the function's name (size_t string length, followed by string bytes).
 *     - Write the function's return type (uint8).
 *     - Write the number of arguments (int32).
 *	   - Write the size of the following argument block (size_t).
 *	   - For each argument;
 *	       - Write an id beginning at 0 (int32).
 *	       - Write the name of the argument (size_t string length, followed by string bytes).
 *	       - Write an argument type.
 *	       
 * - Write the size of the following list of function content blocks (size_t).
 * - Now we write the function content blocks. For each function;
 *     - Write a content block size (size_t).
 *     - Write the node count (int32).
 *     - Write the size in bytes of the serialised block containing nodes (size_t).
 *     - For each node;
 *         - Write the node's ID (int32);
 *         - Write where the node is on the grid (2 int32s);
 *         - Write the node's data packet size (usually zero, but some nodes, like constants, have a state);
 *         - Write the data packet.
 *     - Write the connector arrow count (int32).
 *     - Write the size in bytes of the serialised block containing connector arrows (size_t).
 *     - For each node connector arrow;
 *         - Write the start position (2 int32s)
 *         - Write the target position (2 int32s)
 *         - Write the ID of the argument the connector is connected to on the target (uint8)
 *
 * - Done!
 *
 * To load, we:
 * - Do the reverse of all of that. You can get straight out of here if you think I'm about to write all that out again.
 * 
 * NOTE that the way the data is laid out is conducive to a very specific way of reloading the data - it's important that;
 * - Constants are first loaded into the constant manager;
 * - Then functions are loaded into the function manager from the function headers, where the node manager will be aware of them and resolve them from negative IDs;
 * - Then function bodies are actually populated from the content blocks.
 *     - And within these, all nodes are loaded, THEN all their connections.
 * - Nodes inside functions are loaded LAST (with their connectors being loaded ONLY after all nodes) so that, throughout loading, no data needed to fully initialize a definition can ever be missing. This
 *   relies upon user function call nodes not needing the function being called to have any content upon being created.
 *
 * You should never have to read the ALCH file out of order. The data in one part of the file uses data from the last, and so on.
 */
class Serialiser
{
public:
	
};
