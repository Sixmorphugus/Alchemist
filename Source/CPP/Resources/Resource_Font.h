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

	/** Handles loading the resource files. */
	virtual bool Load(Alchemist* Instance, string FileName) override;

	/** Returns the texture for the given string. */
	SDL_Texture* GetStringTexture(string Name, int Size = 30);

	/** Returns the screen size for the given string. */
	Size GetStringScreenSize(string Name, int Size = 30);

private:
	SDL_Texture* CreateTexture(string Name, int Size = 30);
	TTF_Font* LoadSize(int Size);

private:
	string FontFileName;
	
	unordered_map<int, TTF_Font*> Fonts;
	vector<TTF_Font*> CreatedFontInstances;
	
	unordered_map<int, unordered_map<string, SDL_Texture*>> RenderedStrings;
	unordered_map<int, unordered_map<string, Size>> RenderedStringSizes;
	vector<SDL_Texture*> RenderedStringTextures;

	Alchemist* AInstance;
};
