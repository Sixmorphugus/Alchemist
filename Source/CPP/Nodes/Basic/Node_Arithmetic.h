// Copyright Chris Sixsmith 2020.

#pragma once

#include "Nodes/Nodes.h"
#include "Alchemist.h"
#include "Resources/Resources.h"
#include "Resources/Resource_Font.h"

class BinaryOperatorTraits_Add
{
public:
	static inline string Name = "Add";
	static inline string SymbolChar = "+";
};

class BinaryOperatorTraits_Subtract
{
public:
	static inline string Name = "Subtract";
	static inline string SymbolChar = "-";
};

class BinaryOperatorTraits_Multiply
{
public:
	static inline string Name = "Multiply";
	static inline string SymbolChar = "*";
};

class BinaryOperatorTraits_Divide
{
public:
	static inline string Name = "Divide";
	static inline string SymbolChar = "/";
};

class BinaryOperatorTraits_Equal
{
public:
	static inline string Name = "Equal";
	static inline string SymbolChar = "==";
};

class BinaryOperatorTraits_NotEqual
{
public:
	static inline string Name = "Not Equal";
	static inline string SymbolChar = "/=";
};

class BinaryOperatorTraits_Greater
{
public:
	static inline string Name = "Greater";
	static inline string SymbolChar = ">";
};

class BinaryOperatorTraits_GreaterEqual
{
public:
	static inline string Name = "Greater or Equal";
	static inline string SymbolChar = ">=";
};

class BinaryOperatorTraits_Less
{
public:
	static inline string Name = "Less";
	static inline string SymbolChar = "<";
};

class BinaryOperatorTraits_LessEqual
{
public:
	static inline string Name = "Less or Equal";
	static inline string SymbolChar = "=<";
};

class BinaryOperatorTraits_And
{
public:
	static inline string Name = "And";
	static inline string SymbolChar = "&&";
};

class BinaryOperatorTraits_Or
{
public:
	static inline string Name = "Or";
	static inline string SymbolChar = "||";
};


/**
 * Term nodes are a constant value.
 * There are different kinds for different types.
 */
template<typename OperatorTraits>
class Node_BinaryOperator : public Node
{
public:
	Node_BinaryOperator()
	{
		// Register args.
		RegisterArgument("LeftHandSide");
		RegisterArgument("RightHandSide");
	}
	
	// Node interface.
	virtual shared_ptr<Node> Clone() const override
	{
		return make_shared<Node_BinaryOperator<OperatorTraits>>();
	}
	
	virtual string GetDisplayName() const override
	{
		return OperatorTraits::Name;
	}
	
	virtual string GetCategory() const override
	{
		return "Operators";
	}
	
	virtual void Draw(const Alchemist* Instance, const Point& Position, bool IsPreview = false) const override
	{
		Node::Draw(Instance, Position, IsPreview);

		shared_ptr<Resource_Font> Font = Instance->GetResourceManager()->GetResource<Resource_Font>("Font.ttf");

		SDL_Texture* Tex = Font->GetStringTexture(OperatorTraits::SymbolChar);
		Size Siz = Font->GetStringScreenSize(OperatorTraits::SymbolChar);

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

	virtual bool Emit(string& Output, vector<CompilationProblem>& Problems) override
	{
		bool bLHS = false;
		bool bRHS = false;

		Output += "(";
		
		if(shared_ptr<Node> LHS = GetConnector(0))
		{
			if (LHS->Emit(Output, Problems))
			{
				bLHS = true;
			}
			else
			{
				Problems.push_back("Invalid LHS expression.");
			}
		}

		Output += " " + OperatorTraits::SymbolChar + " ";
		
		if (shared_ptr<Node> RHS = GetConnector(1))
		{
			if (RHS->Emit(Output, Problems))
			{
				bRHS = true;
			}
			else
			{
				Problems.push_back("Invalid RHS expression.");
			}
		}

		Output += ")";
		
		return bLHS && bRHS;
	}
	// End of Node interface.
};
