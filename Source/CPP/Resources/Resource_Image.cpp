// Copyright Chris Sixsmith 2020.

#include "Resources/Resource_Image.h"
#include "Alchemist.h"

Resource_Image::~Resource_Image()
{
	if(Texture)
	{
		SDL_DestroyTexture(Texture);
	}
}

bool Resource_Image::Load(Alchemist* Instance, string FileName)
{
	SDL_Surface* Surface = IMG_Load(("Resources/" + FileName).c_str());

	if(!Surface)
	{
		cout << "Couldn't load image. " << SDL_GetError() << endl;
		return false;
	}

	Texture = SDL_CreateTextureFromSurface(Instance->GetRenderer(), Surface);

	SDL_FreeSurface(Surface);
	
	return true;
}

DECLARE_RESOURCE(Resource_Image, { ".png" });