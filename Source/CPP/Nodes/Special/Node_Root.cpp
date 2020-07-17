// Copyright Chris Sixsmith 2020.

#include "Node_Root.h"
#include "Resources/Resource_Image.h"
#include "Resources/Resource_Font.h"
#include "Module/Function.h"
#include "Alchemist.h"

Node_Root::Node_Root()
{
	// Register return argument.
	RegisterArgument("ReturnValue");

	// Register guard
	RegisterArgument("Guard");
}

shared_ptr<Node> Node_Root::Clone() const
{
	return make_shared<Node_Root>(*this);
}

void Node_Root::Draw(Alchemist* Instance, const Point& Position, bool IsPreview) const
{
	Node::Draw(Instance, Position, IsPreview);

	// Draw fill circle
	shared_ptr<Resource_Image> FillResource = Instance->GetResourceManager()->GetResource<Resource_Image>("NodeFill.png");

	SDL_SetTextureAlphaMod(FillResource->GetTexture(), IsPreview ? 150 : 255);
	SDL_SetTextureColorMod(FillResource->GetTexture(), 0, 0, 0);
	
	SDL_RenderCopy(Instance->GetRenderer(), FillResource->GetTexture(), NULL, &GetRenderRect(Position));

	// Draw @
	shared_ptr<Resource_Image> ArobaseResource = Instance->GetResourceManager()->GetResource<Resource_Image>("NodeIcon_Root.png");

	SDL_SetTextureAlphaMod(ArobaseResource->GetTexture(), IsPreview ? 150 : 255);

	SDL_RenderCopy(Instance->GetRenderer(), ArobaseResource->GetTexture(), NULL, &GetRenderRect(Position));
}

void Node_Root::OnPlaced()
{
	// Create pattern arguments according to function arity.
	for (int i = 0; i < GetFunction()->GetArity(); i++)
	{
		RegisterArgument(string("ArgPattern") + to_string(i), true);
	}
}

DECLARE_NODE(Node_Root);
