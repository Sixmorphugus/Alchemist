// Copyright Chris Sixsmith 2020.

#pragma once

#include "Resources.h"

/** Image Resource class. */
class Resource_Image : public Resource
{
public:
	Resource_Image() = default;
	~Resource_Image();
	
	/** Handles loading the resource file. */
	virtual bool Load(Alchemist* Instance, string FileName) override;

	/** Returns the loaded texture. */
	SDL_Texture* GetTexture() { return Texture; }

private:
	SDL_Texture* Texture = nullptr;
};