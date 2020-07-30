// Copyright Chris Sixsmith 2020.

#include "Resources/Resource_Font.h"
#include "Alchemist.h"

Resource_Font::~Resource_Font()
{
	if (Font)
	{
		TTF_CloseFont(Font);
	}

	for(int i = 0; i < RenderedStringTextures.size(); i++)
	{
		SDL_DestroyTexture(RenderedStringTextures[i]);
	}
}

bool Resource_Font::Load(Alchemist* Instance, string FileName)
{
	Font = TTF_OpenFont(("Resources/" + FileName).c_str(), 30);
	AInstance = Instance;
	
	if (!Font)
	{
		cout << "Couldn't load font. " << SDL_GetError() << endl;
		return false;
	}

	return true;
}

SDL_Texture* Resource_Font::GetStringTexture(string Name)
{
	auto Iterator = RenderedStrings.find(Name);
	if(Iterator != RenderedStrings.end())
	{
		return Iterator->second;
	}

	CreateTexture(Name);
	
	return RenderedStrings[Name];
}

Size Resource_Font::GetStringScreenSize(string Name)
{
	auto Iterator = RenderedStringSizes.find(Name);
	if (Iterator != RenderedStringSizes.end())
	{
		return Iterator->second;
	}

	CreateTexture(Name);

	return RenderedStringSizes[Name];
}

void Resource_Font::CreateTexture(string Name)
{
	// Create a new texture
	SDL_Surface* TextureSurface = TTF_RenderText_Solid(Font, Name.c_str(), SDL_Color{ 255, 255, 255, 255 });

	SDL_Texture* Texture = SDL_CreateTextureFromSurface(AInstance->GetRenderer(), TextureSurface);

	RenderedStringTextures.push_back(Texture);
	RenderedStrings[Name] = Texture;
	RenderedStringSizes[Name] = Size(TextureSurface->w, TextureSurface->h);

	SDL_FreeSurface(TextureSurface);
}

DECLARE_RESOURCE(Resource_Font, { ".ttf" });
