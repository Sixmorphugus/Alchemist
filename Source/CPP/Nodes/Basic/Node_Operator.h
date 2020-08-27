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

class BinaryOperatorTraits_Remainder
{
public:
	static inline string Name = "Remainder";
	static inline string SymbolChar = "rem";
};

class BinaryOperatorTraits_DivideRounded
{
public:
	static inline string Name = "Divide (Rounded)";
	static inline string SymbolChar = "div";
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
	static inline string SymbolChar = "and";
};

class BinaryOperatorTraits_Or
{
public:
	static inline string Name = "Or";
	static inline string SymbolChar = "or";
};

class BinaryOperatorTraits_XOr
{
public:
	static inline string Name = "XOr";
	static inline string SymbolChar = "xor";
};

class BinaryOperatorTraits_BitwiseAnd
{
public:
	static inline string Name = "Bitwise And";
	static inline string SymbolChar = "band";
};

class BinaryOperatorTraits_BitwiseOr
{
public:
	static inline string Name = "Bitwise Or";
	static inline string SymbolChar = "bor";
};

class BinaryOperatorTraits_BitwiseXOr
{
public:
	static inline string Name = "Bitwise XOr";
	static inline string SymbolChar = "bxor";
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

		int ValueTextSize = max(8, 30 - (8 * (max((int)OperatorTraits::SymbolChar.size() - 2, 0))));
		SDL_Texture* Tex = Font->GetStringTexture(OperatorTraits::SymbolChar, ValueTextSize);
		Size Siz = Font->GetStringScreenSize(OperatorTraits::SymbolChar, ValueTextSize);

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

	virtual bool EmitInternal(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path) override
	{
		bool bLHS = false;
		bool bRHS = false;

		Output += "(";
		
		if(shared_ptr<Node> LHS = GetConnector(0))
		{
			if (LHS->Emit(Output, Problems, Path))
			{
				bLHS = true;
			}
		}
		else
		{
			Problems.push_back(CompilationProblem{ shared_from_this(), "Missing LHS expression." });
		}

		Output += " " + OperatorTraits::SymbolChar + " ";
		
		if (shared_ptr<Node> RHS = GetConnector(1))
		{
			if (RHS->Emit(Output, Problems, Path))
			{
				bRHS = true;
			}
		}
		else
		{
			Problems.push_back(CompilationProblem{ shared_from_this(), "Missing RHS expression." });
		}

		Output += ")";
		
		return bLHS && bRHS;
	}
	// End of Node interface.
};


class UnaryOperatorTraits_Not
{
public:
	static inline string Name = "Not";
	static inline string SymbolChar = "not";
};

class BinaryOperatorTraits_BitwiseNot
{
public:
	static inline string Name = "Bitwise Not";
	static inline string SymbolChar = "bnot";
};


/**
 * Term nodes are a constant value.
 * There are different kinds for different types.
 */
template<typename OperatorTraits>
class Node_UnaryOperator : public Node
{
public:
	Node_UnaryOperator()
	{
		// Register args.
		RegisterArgument("Input");
	}

	// Node interface.
	virtual shared_ptr<Node> Clone() const override
	{
		return make_shared<Node_UnaryOperator<OperatorTraits>>();
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

		int ValueTextSize = max(8, 30 - (8 * (max((int)OperatorTraits::SymbolChar.size() - 2, 0))));
		SDL_Texture* Tex = Font->GetStringTexture(OperatorTraits::SymbolChar, ValueTextSize);
		Size Siz = Font->GetStringScreenSize(OperatorTraits::SymbolChar, ValueTextSize);

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

	virtual bool EmitInternal(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path) override
	{
		bool bInput = false;

		Output += "(";

		Output += OperatorTraits::SymbolChar + " ";

		if (shared_ptr<Node> Input = GetConnector(1))
		{
			if (Input->Emit(Output, Problems, Path))
			{
				bInput = true;
			}
		}
		else
		{
			Problems.push_back(CompilationProblem{ shared_from_this(), "Missing Input expression." });
		}

		Output += ")";

		return bInput;
	}
	// End of Node interface.
};
