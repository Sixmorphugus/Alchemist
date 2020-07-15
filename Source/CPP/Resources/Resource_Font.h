// Copyright Chris Sixsmith 2020.

#pragma once

#include "Resources.h"

/** Font Resource class. */
class Resource_Font : public Resource
{
public:
	Resource_Font() = default;
	~Resource_Font();

	/** Handles loading the resource file. */
	virtual bool Load(Alchemist* Instance, string FileName) override;

	/** Returns the loaded texture. */
	TTF_Font* GetFont() { return Font; }

private:
	TTF_Font* Font = nullptr;
};
