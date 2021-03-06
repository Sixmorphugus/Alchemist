// Copyright Chris Sixsmith 2020.

#include "Node_Root.h"
#include "Resources/Resource_Image.h"
#include "Resources/Resource_Font.h"
#include "Module/Function.h"
#include "Alchemist.h"

Node_Root::Node_Root()
{
	
}

shared_ptr<Node> Node_Root::Clone() const
{
	return make_shared<Node_Root>(*this);
}

void Node_Root::Draw(const Alchemist* Instance, const Point& Position, bool IsPreview) const
{
	Node::Draw(Instance, Position, IsPreview);

	SDL_Rect Rect = GetRenderRect(Position);
	
	// Draw fill circle
	shared_ptr<Resource_Image> FillResource = Instance->GetResourceManager()->GetResource<Resource_Image>("NodeFill.png");

	SDL_SetTextureAlphaMod(FillResource->GetTexture(), IsPreview ? 150 : 255);
	SDL_SetTextureColorMod(FillResource->GetTexture(), 0, 0, 0);
	
	SDL_RenderCopy(Instance->GetRenderer(), FillResource->GetTexture(), NULL, &Rect);

	// Draw @
	shared_ptr<Resource_Image> ArobaseResource = Instance->GetResourceManager()->GetResource<Resource_Image>("NodeIcon_Root.png");

	SDL_SetTextureAlphaMod(ArobaseResource->GetTexture(), IsPreview ? 150 : 255);

	SDL_RenderCopy(Instance->GetRenderer(), ArobaseResource->GetTexture(), NULL, &Rect);
}

bool Node_Root::EmitInternal(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path)
{
	// Root nodes emit a full function definition minus the terminating character (. or ;) which is handled by the function emit function.
	// First we emit the function header. This includes:
	// - The argument binding patterns.
	// - The guard expression.

	bool Success = true;
	
	// First emit function name
	Output += GetFunction()->GetName();

	// Then arg expressions, one by one (missing arg expression = error and exit)
	Output += "(";

	for (int i = 0; i < GetFunction()->GetArity(); i++)
	{
		// Try to find arg-representing node
		shared_ptr<Node> ArgPatternArg = GetConnector(i + 2);

		if(!ArgPatternArg)
		{
			Problems.push_back(CompilationProblem{ shared_from_this(), "Argument " + to_string(i) + " was not defined." });
			Success = false;
		}
		else
		{
			// Emit it
			if(!ArgPatternArg->Emit(Output, Problems, Path))
			{
				Success = false;
			}
		}

		// Comma if required
		if(i + 1 < GetFunction()->GetArity())
		{
			Output += ", ";
		}
	}
	
	Output += ") ";

	// Now emit the guard sequence.
	// The guard sequence is allowed to be unconnected.
	if(shared_ptr<Node> Guard = GetConnector(1))
	{
		Output += "when ";
		
		if(!Guard->Emit(Output, Problems, Path))
		{
			Success = false;
		}

		Output += " ";
	}

	Output += "->\n\t";

	// Output the nested expression.
	if (shared_ptr<Node> Expression = GetConnector(0))
	{
		if(!Expression->Emit(Output, Problems, Path))
		{
			Success = false;
		}
	}
	else
	{
		Problems.push_back(CompilationProblem{ shared_from_this(), "Return value was not defined." });
		Success = false;
	}

	return Success;
}

void Node_Root::OnFunctionChanged()
{
	// Store connectors
	vector<shared_ptr<Node>> Connectors;
	
	for(int i = 0; i < GetNumArguments(); i++)
	{
		Connectors.push_back(GetConnector(i));
	}

	// Do the deed
	ClearArguments();
	
	// Register return argument.
	RegisterArgument("ReturnValue");

	// Register guard
	RegisterArgument("Guard");
	
	// Create pattern arguments according to function arity.
	for (int i = 0; i < GetFunction()->GetArity(); i++)
	{
		RegisterArgument("ArgPattern" + to_string(i), true);
	}

	// Re-load stored arguments
	for (int i = 0; i < Connectors.size() && i < GetNumArguments(); i++)
	{
		SetConnector(Connectors[i], i);
	}
}

DECLARE_NODE(Node_Root);
