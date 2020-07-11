// Copyright Chris Sixsmith 2020.

#include "Resources.h"

static vector<ResourceFactoryBase*> StaticFactories;


void ResourceManager::LoadResources(Alchemist* Instance)
{
	// Iterate the resources folder and look for a factory that supports the extension for each file we find.
	// When we get a match, load the resource.
	for (const auto& p : filesystem::directory_iterator("Resources"))
	{
		cout << p.path() << endl;
		
		for (ResourceFactoryBase* Factory : StaticFactories)
		{
			for (const string& Extension : Factory->GetSupportedFileExtensions())
			{
				if (Extension == p.path().extension())
				{
					string Name = p.path().filename().string();
					if (shared_ptr<Resource> Resource = Factory->CreateResource(Instance, Name, p.path().filename().string()))
					{
						assert(Resources.find(Name) == Resources.end()); // make sure no two resources exist with same filename (actually impossible tho)
						Resources[Name] = Resource;

						cout << "Loaded resource \"" << Name << "\"." << endl;
					}
					else
					{
						cout << "Failed to load resource \"" << Name << "\"!" << endl;
					}

					break; // make sure not to bother checking other file extensions.
				}
			}
		}
	}
}

shared_ptr<Resource> ResourceManager::GetResource(string Name)
{
	assert(Resources.find(Name) != Resources.end());
	return Resources[Name];
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


ResourceRegistrar::ResourceRegistrar(ResourceFactoryBase* FactoryIn)
{
	StaticFactories.push_back(FactoryIn);
}