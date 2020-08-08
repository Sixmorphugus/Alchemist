// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"

class Alchemist;
class Function;

class Module
{
public:
	Module(Alchemist* InstanceIn, string NameIn = "untitled");

	/** Creates or gets a new function. */
	shared_ptr<Function> CreateOrGetFunction(string Name, int Arity);

	/** Creates a unique function. */
	shared_ptr<Function> CreateUniqueFunction();

	/** Gets an existing function, const version. */
	const shared_ptr<Function> GetFunction(string Name) const;

	/** Gets an existing function. */
	shared_ptr<Function> GetFunction(string Name);

	/** Removes a function. */
	void RemoveFunction(string Name);

	/** Returns the module's full function list. */
	vector<shared_ptr<Function>> GetFunctions() const { return Functions; }

	/** Returns module's name. */
	string GetName() const { return Name; }

	/** Returns alchemist application instance. */
	Alchemist* GetInstance() const { return Instance; }

	/** Rebuilds function lookup. */
	void UpdateLookups();

protected:
	/** Handles module change. */
	void BroadcastOnModuleChanged();

private:
	Alchemist* Instance;
	vector<shared_ptr<Function>> Functions;
	unordered_map<string, int> FunctionLookupTable;

	string Name;
};