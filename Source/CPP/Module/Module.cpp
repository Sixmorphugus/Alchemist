// Copyright Chris Sixsmith 2020.

#include "Module.h"
#include "Alchemist.h"
#include "Function.h"

Module::Module(Alchemist* InstanceIn, string NameIn)
	: Instance(InstanceIn), Name(NameIn)
{}

shared_ptr<Function> Module::CreateOrGetFunction(string Name, int Arity)
{
	if (shared_ptr<Function> Existing = GetFunction(Name))
	{
		return Existing;
	}

	shared_ptr<Function> NewFunction = make_shared<Function>(Instance, Name, Arity);
	NewFunction->ParentModule = this;

	Functions.push_back(NewFunction);
	FunctionLookupTable[Name] = (int)Functions.size() - 1;
	
	return NewFunction;
}

const shared_ptr<Function> Module::GetFunction(string Name) const
{
	auto Found = FunctionLookupTable.find(Name);
	if (Found != FunctionLookupTable.end())
	{
		return Functions[Found->second];
	}

	return nullptr;
}

shared_ptr<Function> Module::GetFunction(string Name)
{
	auto Found = FunctionLookupTable.find(Name);
	if (Found != FunctionLookupTable.end())
	{
		return Functions[Found->second];
	}

	return nullptr;
}

void Module::RemoveFunction(string Name)
{
	// TODO
}

