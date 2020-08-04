// Copyright Chris Sixsmith 2020.

#include "Node_Variable.h"
#include "Resources/Resource_Font.h"
#include "Alchemist.h"

shared_ptr<Node> Node_Variable::Clone() const
{
	return make_shared<Node_Variable>(*this);
}

void Node_Variable::Draw(Alchemist* Instance, const Point& Position, bool IsPreview) const
{
	Node::Draw(Instance, Position, IsPreview);

	shared_ptr<Resource_Font> Font = Instance->GetResourceManager()->GetResource<Resource_Font>("Font.ttf");

	if(Name != "")
	{
		SDL_Texture* Tex = Font->GetStringTexture(Name);
		Size Siz = Font->GetStringScreenSize(Name);

		SDL_SetTextureColorMod(Tex, 0, 0, 0);

		SDL_Rect TexDestRect
		{
			Position.X + 32 - (Siz.X / 2),
			Position.Y + 16,
			Siz.X,
			Siz.Y
		};

		SDL_RenderCopy(Instance->GetRenderer(), Tex, NULL, &TexDestRect);
	}
}

bool Node_Variable::Emit(string& Output, vector<CompilationProblem>& Problems)
{
	// TODO catch first letter not being uppercase
	
	Output += Name;

	return true;
}

DECLARE_NODE(Node_Variable);
