// Copyright Chris Sixsmith 2020.

#include "Node_Term.h"
#include "Resources/Resource_Font.h"
#include "Alchemist.h"
#include "Resources/Resource_Image.h"

shared_ptr<Node> Node_Term_Int::Clone() const
{
	return make_shared<Node_Term_Int>(*this);
}

void Node_Term_Int::Draw(const Alchemist* Instance, const Point& Position, bool IsPreview) const
{
	shared_ptr<Resource_Image> NodeResource = Instance->GetResourceManager()->GetResource<Resource_Image>("Node.png");

	assert(NodeResource);

	SDL_Rect Rect = GetRenderRect(Position);
	
	SDL_SetTextureAlphaMod(NodeResource->GetTexture(), IsPreview ? 150 : 255);
	SDL_RenderCopy(Instance->GetRenderer(), NodeResource->GetTexture(), NULL, &Rect);
	
	//Node::Draw(Instance, Position, IsPreview);

	string ValueText = to_string(Value);
	
	shared_ptr<Resource_Font> Font = Instance->GetResourceManager()->GetResource<Resource_Font>("Font.ttf");
	
	SDL_Texture* Tex = Font->GetStringTexture(ValueText);
	Size Siz = Font->GetStringScreenSize(ValueText);

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

void Node_Term_Int::Load(const ifstream& FileStream)
{
	// TODO
}

void Node_Term_Int::Save(const ofstream& FileStream) const
{
	// TODO
}

size_t Node_Term_Int::GetDataSize() const
{
	return sizeof(int);
}

bool Node_Term_Int::EmitInternal(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path)
{
	Output += to_string(Value);

	return true;
}

void Node_Term_Int::HandleTextInput(const SDL_Event& Event)
{
	string ValueStr = to_string(Value);
	ValueStr += Event.text.text;

	Value = atoi(ValueStr.c_str());
}

void Node_Term_Int::HandleKeyPress(const SDL_Event& Event)
{
	if (Event.key.keysym.sym == SDLK_BACKSPACE)
	{
		string ValueStr = to_string(Value);
		ValueStr = ValueStr.substr(0, ValueStr.size() - 1);

		Value = atoi(ValueStr.c_str());
	}
}


shared_ptr<Node> Node_Term_Bool::Clone() const
{
	return make_shared<Node_Term_Bool>(*this);
}

void Node_Term_Bool::Draw(const Alchemist* Instance, const Point& Position, bool IsPreview) const
{
	shared_ptr<Resource_Image> NodeResource = Instance->GetResourceManager()->GetResource<Resource_Image>("Node.png");

	assert(NodeResource);

	SDL_Rect Rect = GetRenderRect(Position);

	SDL_SetTextureAlphaMod(NodeResource->GetTexture(), IsPreview ? 150 : 255);
	SDL_RenderCopy(Instance->GetRenderer(), NodeResource->GetTexture(), NULL, &Rect);

	//Node::Draw(Instance, Position, IsPreview);

	string ValueText = Value ? "true" : "false";

	shared_ptr<Resource_Font> Font = Instance->GetResourceManager()->GetResource<Resource_Font>("Font.ttf");

	SDL_Texture* Tex = Font->GetStringTexture(ValueText, 16);
	Size Siz = Font->GetStringScreenSize(ValueText, 16);

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

void Node_Term_Bool::Load(const ifstream& FileStream)
{
	// ...
}

void Node_Term_Bool::Save(const ofstream& FileStream) const
{
	// ...
}

size_t Node_Term_Bool::GetDataSize() const
{
	return sizeof(bool);
}

bool Node_Term_Bool::EmitInternal(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path)
{
	Output += Value ? "true" : "false";

	return true;
}

void Node_Term_Bool::HandleTextInput(const SDL_Event& Event)
{
	Value = true;
}

void Node_Term_Bool::HandleKeyPress(const SDL_Event& Event)
{
	if (Event.key.keysym.sym == SDLK_BACKSPACE)
	{
		Value = false;
	}
}


DECLARE_NODE(Node_Term_Bool);
DECLARE_NODE(Node_Term_Int);
