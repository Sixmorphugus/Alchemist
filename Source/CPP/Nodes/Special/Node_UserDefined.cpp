// Copyright Chris Sixsmith 2020.

#include "Node_UserDefined.h"
#include "Module/Function.h"

Node_UserDefined::Node_UserDefined(shared_ptr<Function> FuncIn)
	: Func(FuncIn)
{
	SetupArgs();
}

string Node_UserDefined::GetDisplayName() const
{
	if(Func.expired())
	{
		return "BADFUNC!!";
	}
	
	return Func.lock()->GetName();
}

shared_ptr<Node> Node_UserDefined::Clone() const
{
	return make_shared<Node_UserDefined>(*this);
}

void Node_UserDefined::Draw(const Alchemist* Instance, const Point& Position, bool IsPreview) const
{
	Node::Draw(Instance, Position, IsPreview);

	if (!Func.expired())
	{
		// Draw the first letter of the function's name.
		string Letter = Func.lock()->GetName().substr(0, 1);

		shared_ptr<Resource_Font> Font = Instance->GetResourceManager()->GetResource<Resource_Font>("Font.ttf");

		Size LSize = Font->GetStringScreenSize(Letter);
		SDL_Texture* LTexture = Font->GetStringTexture(Letter);

		SDL_SetTextureColorMod(LTexture, 0, 0, 0);

		SDL_Rect LRect = {
			Position.X + (GridSize / 2) - (LSize.X / 2),
			Position.Y + (GridSize / 2) - (LSize.Y / 2),
			LSize.X,
			LSize.Y
		};

		SDL_RenderCopy(Instance->GetRenderer(), LTexture, NULL, &LRect);
	}
}

bool Node_UserDefined::EmitInternal(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path)
{
	if(Func.expired())
	{
		Problems.push_back(CompilationProblem{ shared_from_this(), "Referenced function was deleted!" });
		return false;
	}

	Output += Func.lock()->GetName() + "(";

	bool Success = true;
	
	for (int i = 0; i < Func.lock()->GetArity(); i++)
	{
		shared_ptr<Node> Connector = GetConnector(i);

		if(Connector)
		{
			Connector->Emit(Output, Problems, Path);
		}
		else
		{
			Problems.push_back(CompilationProblem{ shared_from_this(), "Required argument " + to_string(i+1) + " missing." });
			Success = false;
		}

		if(i + 1 < Func.lock()->GetArity())
		{
			Output += ", ";
		}
	}

	Output += ")";
	
	return Success;
}

void Node_UserDefined::OnModuleChanged()
{
	// Self-destruct if our target is removed from the module.
	if(Func.expired())
	{
		GetFunction()->RemoveNode(shared_from_this());
	}
	else
	{
		if (GetNumArguments() != Func.lock()->GetArity())
		{
			SetupArgs();
		}
	}
}

void Node_UserDefined::SetupArgs()
{
	// Create all needed args
	for (int i = 0; i < Func.lock()->GetArity(); i++)
	{
		RegisterArgument("Arg" + to_string(i+1));
	}
}
