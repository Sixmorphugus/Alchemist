// Copyright Chris Sixsmith 2020.

#include "Resources/Resource_Font.h"
#include "Alchemist.h"

Resource_Font::~Resource_Font()
{
	if (Font)
	{
		TTF_CloseFont(Font);
	}
}

bool Resource_Font::Load(Alchemist* Instance, string FileName)
{
	Font = TTF_OpenFont(("Resources/" + FileName).c_str(), 12);

	if (!Font)
	{
		cout << "Couldn't load font. " << SDL_GetError() << endl;
		return false;
	}

	return true;
}

DECLARE_RESOURCE(Resource_Font, { ".ttf" });
