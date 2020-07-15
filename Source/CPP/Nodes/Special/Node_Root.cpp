// Copyright Chris Sixsmith 2020.

#include "Node_Root.h"
#include "Resources/Resource_Image.h"
#include "Resources/Resource_Font.h"
#include "Alchemist.h"

shared_ptr<Node> Node_Root::Clone() const
{
	return make_shared<Node_Root>(*this);
}

string Node_Root::GetDisplayName() const
{
	return "Root";
}

VarType Node_Root::GetReturnType() const
{
	return VarType::Invalid;
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

DECLARE_NODE(Node_Root);
