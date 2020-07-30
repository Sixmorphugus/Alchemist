// Copyright Chris Sixsmith 2020.

#pragma once

#include "2DPositioning.h"
#include "Resources.h"

// TODO generation of font textures, font texture cache

/** Font Resource class. */
class Resource_Font : public Resource
{
public:
	Resource_Font() = default;
	~Resource_Font();

	/** Handles loading the resource file. */
	virtual bool Load(Alchemist* Instance, string FileName) override;

	/** Returns the loaded font. */
	TTF_Font* GetFont() const { return Font; }

	/** Returns the texture for the given string. */
	SDL_Texture* GetStringTexture(string Name);

	/** Returns the screen size for the given string. */
	Size GetStringScreenSize(string Name);

private:
	void CreateTexture(string Name);

private:
	TTF_Font* Font = nullptr;
	
	unordered_map<string, SDL_Texture*> RenderedStrings;
	unordered_map<string, Size> RenderedStringSizes;
	vector<SDL_Texture*> RenderedStringTextures;

	Alchemist* AInstance;
};
