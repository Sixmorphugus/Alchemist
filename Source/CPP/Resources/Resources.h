// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"

class Alchemist;
class ResourceManager;


/** Resource class. */
class Resource
{
public:
	/** Handles loading the resource file. */
	virtual bool Load(Alchemist* Instance, string FileName) = 0;

	/** Returns the resource's name, set by the resource factory. */
	string GetName() const { return Name; }

private:
	string Name;

	friend class ResourceFactoryBase;
};


/** Resource manager. Loads resources found in the Resources folder. */
class ResourceManager
{
public:
	ResourceManager() = default;

	void LoadResources(Alchemist* Instance);

	shared_ptr<Resource> GetResource(string Name);

	template<class ResourceClass>
	shared_ptr<ResourceClass> GetResource(string Name)
	{
		shared_ptr<Resource> Resource = GetResource(Name);
		return dynamic_pointer_cast<ResourceClass>(Resource);
	}

private:
	unordered_map<string, shared_ptr<Resource>> Resources;
};


/**
 * Resource factory.
 * Tells the ResourceManager how to create resources.
 */
class ResourceFactoryBase
{
public:
	ResourceFactoryBase(vector<string> FileExtensions)
		: SupportedFileExtensions(FileExtensions)
	{}

	vector<string> GetSupportedFileExtensions() const { return SupportedFileExtensions; }
	virtual shared_ptr<Resource> CreateResource(Alchemist* Instance, string ResourceName, string FileName) const = 0;

	template<class ResourceClass>
	void DoNameHack(shared_ptr<ResourceClass> Resource, string ResourceName) const
	{
		Resource->Name = ResourceName;
	}
	
private:
	vector<string> SupportedFileExtensions;
};

template<class ResourceClass>
class ResourceFactory : public ResourceFactoryBase
{
public:
	ResourceFactory(vector<string> FileExtensions)
		: ResourceFactoryBase(FileExtensions)
	{}

	shared_ptr<Resource> CreateResource(Alchemist* Instance, string ResourceName, string FileName) const override
	{
		shared_ptr<ResourceClass> Resource = make_shared<ResourceClass>();
		DoNameHack(Resource, ResourceName);
		
		if(Resource->Load(Instance, FileName))
		{
			return Resource;
		}

		return nullptr;
	}
};


/** ResourceRegistrar. Ditto, but at compile time. */
class ResourceRegistrar
{
public:
	ResourceRegistrar(ResourceFactoryBase* FactoryIn);
};

#define DECLARE_RESOURCE(ResourceClass, ResourceExtensionList) ResourceRegistrar ResourceClass ## Def(new ResourceFactory<ResourceClass>(ResourceExtensionList))
