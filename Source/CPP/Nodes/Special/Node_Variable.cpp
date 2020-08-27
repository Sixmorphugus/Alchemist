// Copyright Chris Sixsmith 2020.

#include "Node_Variable.h"
#include "Resources/Resource_Font.h"
#include "Alchemist.h"
#include "Resources/Resource_Image.h"

shared_ptr<Node> Node_Variable::Clone() const
{
	return make_shared<Node_Variable>(*this);
}

void Node_Variable::Draw(const Alchemist* Instance, const Point& Position, bool IsPreview) const
{
	shared_ptr<Resource_Image> NodeResource = Instance->GetResourceManager()->GetResource<Resource_Image>("Node.png");

	assert(NodeResource);

	SDL_Rect Rect = GetRenderRect(Position);

	SDL_SetTextureAlphaMod(NodeResource->GetTexture(), IsPreview ? 150 : 255);
	SDL_RenderCopy(Instance->GetRenderer(), NodeResource->GetTexture(), NULL, &Rect);

	shared_ptr<Resource_Font> Font = Instance->GetResourceManager()->GetResource<Resource_Font>("Font.ttf");

	if(Name != "")
	{
		int ValueTextSize = max(8, 30 - (6 * (max((int)Name.size() - 2, 0))));
		
		SDL_Texture* Tex = Font->GetStringTexture(Name, ValueTextSize);
		Size Siz = Font->GetStringScreenSize(Name, ValueTextSize);

		SDL_SetTextureColorMod(Tex, 0, 0, 0);

		SDL_Rect TexDestRect
		{
			Position.X + 32 - (Siz.X / 2),
			Position.Y + 32 - (Siz.Y / 2),
			Siz.X,
			Siz.Y
		};

		SDL_RenderCopy(Instance->GetRenderer(), Tex, NULL, &TexDestRect);
	}
}

bool Node_Variable::EmitInternal(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path)
{
	if(Name.size() == 0)
	{
		Problems.push_back(CompilationProblem{ shared_from_this(), "No variable name was provided." });
		return false;
	}

	if(!isupper(Name[0]))
	{
		Problems.push_back(CompilationProblem{ shared_from_this(), "Variable names must start with a capital letter to be valid Erlang." });
		return false;
	}
	
	Output += Name;

	return true;
}

void Node_Variable::HandleTextInput(const SDL_Event& Event)
{
	Name += Event.text.text;
}

void Node_Variable::HandleKeyPress(const SDL_Event& Event)
{
	if(Event.key.keysym.sym == SDLK_BACKSPACE)
	{
		Name = Name.substr(0, Name.size() - 1);
	}
}

DECLARE_NODE(Node_Variable);
