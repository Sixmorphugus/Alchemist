// Copyright Chris Sixsmith 2020.

#include "Resources/Resource_Font.h"
#include "Alchemist.h"

Resource_Font::~Resource_Font()
{
	for(int i = 0; i < RenderedStringTextures.size(); i++)
	{
		SDL_DestroyTexture(RenderedStringTextures[i]);
	}

	for (int i = 0; i < CreatedFontInstances.size(); i++)
	{
		TTF_CloseFont(CreatedFontInstances[i]);
	}
}

bool Resource_Font::Load(Alchemist* Instance, string FileName)
{
	AInstance = Instance;
	FontFileName = FileName;
	
	if (!LoadSize(30)) // this is the only time we actually make sure we can load the font
	{
		cout << "Couldn't load font. " << SDL_GetError() << endl;
		return false;
	}

	return true;
}

SDL_Texture* Resource_Font::GetStringTexture(string Name, int Size)
{
	auto StringTableFindAttempt = RenderedStrings.find(Size);

	if(StringTableFindAttempt != RenderedStrings.end())
	{
		auto StringFindAttempt = StringTableFindAttempt->second.find(Name);

		if(StringFindAttempt != StringTableFindAttempt->second.end())
		{
			return StringFindAttempt->second;
		}
	}
	
	return CreateTexture(Name, Size);
}

Size Resource_Font::GetStringScreenSize(string Name, int Size)
{
	auto StringTableFindAttempt = RenderedStringSizes.find(Size);

	if (StringTableFindAttempt != RenderedStringSizes.end())
	{
		auto StringFindAttempt = StringTableFindAttempt->second.find(Name);

		if (StringFindAttempt != StringTableFindAttempt->second.end())
		{
			return StringFindAttempt->second;
		}
	}

	// horrible but it works fine
	assert(CreateTexture(Name, Size) != nullptr);
	return GetStringScreenSize(Name, Size);
}

SDL_Texture* Resource_Font::CreateTexture(string Name, int Size)
{
	auto FontFindAttempt = Fonts.find(Size);

	TTF_Font* Font = nullptr;

	if(FontFindAttempt != Fonts.end())
	{
		Font = FontFindAttempt->second;
	}
	else
	{
		Font = LoadSize(Size);
	}

	if(!Font)
	{
		return nullptr;
	}
	
	// Create a new texture
	SDL_Surface* TextureSurface = TTF_RenderText_Solid(Font, Name.c_str(), SDL_Color{ 255, 255, 255, 255 });

	SDL_Texture* Texture = SDL_CreateTextureFromSurface(AInstance->GetRenderer(), TextureSurface);

	RenderedStringTextures.push_back(Texture);
	
	RenderedStrings[Size][Name] = Texture;
	RenderedStringSizes[Size][Name] = ::Size(TextureSurface->w, TextureSurface->h);

	SDL_FreeSurface(TextureSurface);

	return Texture;
}

TTF_Font* Resource_Font::LoadSize(int Size)
{
	TTF_Font* FontInstance = TTF_OpenFont(("Resources/" + FontFileName).c_str(), Size);

	Fonts[Size] = FontInstance;
	CreatedFontInstances.push_back(FontInstance);

	RenderedStrings[Size] = unordered_map<string, SDL_Texture*>();
	RenderedStringSizes[Size] = unordered_map<string, ::Size>();
	
	return FontInstance;
}

DECLARE_RESOURCE(Resource_Font, { ".ttf" });
